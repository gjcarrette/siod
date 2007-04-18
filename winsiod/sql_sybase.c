/* 
 Name:    sql_sybase.c
 Purpose: scheme interpreter interface to Sybase CT Library.

 Modification history
 06/01/95    George Carrette.  Initial implementation.

 Note: 99% of the time we only need one connection to the database,
       therefore the variable *sybase* will serve to keep a default
       first argument to most procedures.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <ctpublic.h>
#include "siod.h"

#ifdef SUN5
int gethostname(char *, int);   
#endif

static void init_sql_sybase_version(void)
{setvar(cintern("*sql_sybase-version*"),
	cintern("$Id: sql_sybase.c,v 1.7 1997/12/01 15:23:17 gjc Exp $"),
	NIL);}

char *sybase_retstr(CS_RETCODE n);
char *sybase_typestr(CS_INT n);
void init_sql_sybasec(void);

long tc_sybase_state = 0;

LISP sym_sybase = NIL;
LISP sym_sybase_messages = NIL;

static void complain(char *msg, ...)
{va_list args;
 char msgbuff[1024];
 if (siod_verbose_check(1))
   {va_start(args,msg);
    vsprintf(msgbuff,msg,args);
    va_end(args);
    fprintf(stdout,"\nSYBASE: %s\n",msgbuff);}}

struct sybase_state
{CS_CONTEXT *cp;
 CS_COMMAND *cmd;
 CS_CONNECTION *conn;
 long connected;
 long inited;};

static CS_RETCODE sybase_teardown(struct sybase_state *);
static CS_RETCODE cs_err_handler(CS_CONTEXT *,CS_CLIENTMSG *);
static CS_RETCODE client_err_handler(CS_CONTEXT *,
				     CS_CONNECTION *,
				     CS_CLIENTMSG *);
static CS_RETCODE server_err_handler(CS_CONTEXT *,
				     CS_CONNECTION *,
				     CS_SERVERMSG *);

static CS_RETCODE sybase_setup(struct sybase_state *sybase_state,
			       char *sybase_user,
			       char *sybase_pw,
			       char *sybase_app)
{CS_RETCODE status = CS_SUCCEED;
 char hostname[128];
 memset(sybase_state,0,sizeof(struct sybase_state));
 status = cs_ctx_alloc(CS_VERSION_100, &sybase_state->cp);
 if (status != CS_SUCCEED)
   {complain("cs_ctx_alloc %d",status);
    sybase_teardown(sybase_state);
    return(status);}
 status = cs_config(sybase_state->cp,CS_SET,CS_MESSAGE_CB,
		    cs_err_handler,CS_UNUSED,NULL);
 if (status != CS_SUCCEED)
   {complain("cs_config CS_MESSAGE_CB %d",status);
    sybase_teardown(sybase_state);
    return(status);}
 status = ct_init(sybase_state->cp, CS_VERSION_100);
 if (status != CS_SUCCEED)
   {complain("ct_init %d",status);
    sybase_teardown(sybase_state);
    return(status);}
 sybase_state->inited = 1;
 status = ct_callback(sybase_state->cp,(CS_CONNECTION *)NULL, CS_SET,
		      CS_CLIENTMSG_CB, client_err_handler);
 if (status != CS_SUCCEED)
   {complain("CS_CLIENTMSG_CB %d",status);
    sybase_teardown(sybase_state);
    return(status);}
 status = ct_callback(sybase_state->cp,(CS_CONNECTION *)NULL, CS_SET,
		      CS_SERVERMSG_CB, server_err_handler);
 if (status != CS_SUCCEED)
   {complain("CS_SERVERMSG_CB %d",status);
    sybase_teardown(sybase_state);
    return(status);}
 status = ct_con_alloc(sybase_state->cp, &sybase_state->conn);
 if (status != CS_SUCCEED)
   {complain("ct_con_alloc %d", status); 
    sybase_teardown(sybase_state);
    return(status);}
 status = ct_con_props(sybase_state->conn, CS_SET, CS_USERNAME,
		       sybase_user, 
		       CS_NULLTERM, (CS_INT *)NULL);
 if (status != CS_SUCCEED)
   {complain("ct_con_props CS_USERNAME %d",status);
    sybase_teardown(sybase_state);
    return(status);}
 status = ct_con_props(sybase_state->conn, CS_SET, CS_PASSWORD,
		       sybase_pw, 
		       CS_NULLTERM, (CS_INT *) NULL);
 if (status != CS_SUCCEED)
   {complain("ct_con_props CS_PASSWORD %d",status);
    sybase_teardown(sybase_state);
    return(status);}
 status = ct_con_props(sybase_state->conn, CS_SET, CS_APPNAME,
		       sybase_app, 
		       CS_NULLTERM, (CS_INT *) NULL);
 if (status != CS_SUCCEED)
   {complain("ct_con_props CS_APPNAME %d",status);
    sybase_teardown(sybase_state);
    return(status);}
#ifndef VMS
 if (gethostname(hostname,sizeof(hostname)) == 0)
   {status = ct_con_props(sybase_state->conn, CS_SET, CS_HOSTNAME,
			  hostname,
			  CS_NULLTERM, (CS_INT *) NULL);
    if (status != CS_SUCCEED)
      {complain("ct_con_props CS_HOSTNAME %d",status);
       sybase_teardown(sybase_state);
       return(status);}}
#endif
 status = ct_connect(sybase_state->conn, NULL, 0);
 if (status != CS_SUCCEED)
   {complain("ct_connent(%s,%s,%s) %d",
	     sybase_user,sybase_pw,
	     sybase_app,status);
    sybase_teardown(sybase_state);
    return(status);}
 sybase_state->connected = 1;
 status = ct_cmd_alloc(sybase_state->conn, &sybase_state->cmd);
 if (status != CS_SUCCEED)
   {complain("ct_cmd_alloc %d",status);
    sybase_teardown(sybase_state);
    return(status);}
 return(status);}

static CS_RETCODE sybase_teardown(struct sybase_state *sybase_state)
{CS_RETCODE status = CS_SUCCEED;
 if (sybase_state->cmd)
   {status = ct_cmd_drop(sybase_state->cmd);
    sybase_state->cmd = NULL;}
 if (sybase_state->connected)
   {status = ct_close(sybase_state->conn, CS_FORCE_CLOSE);
    sybase_state->connected = 0;}
 if (sybase_state->conn)
   {status = ct_con_drop(sybase_state->conn);
    sybase_state->conn = NULL;}
 if (sybase_state->inited)
   {status = ct_exit(sybase_state->cp, CS_FORCE_EXIT);
    sybase_state->inited = 0;}
 if (sybase_state->cp)
   {status = cs_ctx_drop(sybase_state->cp);
    sybase_state->cp = NULL;}
 return(status);}

static char *getarg(LISP l,char *name,char *dflt)
{LISP cell,key;
 key = rintern(name);
 for(cell=l;NNULLP(cell);cell=cdr(cdr(cell)))
   if EQ(car(cell),key)
     return(get_c_string(cadr(cell)));
 return(dflt);}

static LISP statcons(CS_RETCODE n)
{char *str;
 if ((str = sybase_retstr(n)))
   return(cintern(str));
 else
   return(flocons(n));}

struct sybase_state *get_sybase_state(LISP ptr,long openp)
{struct sybase_state *s;
 if TYPEP(ptr,tc_sybase_state)
   {if ((s = (struct sybase_state *) ptr->storage_as.string.data))
     return(s);
    else if (openp)
      {err("sybase connection not open",ptr);
       return(NULL);}
    else
      return(NULL);}
 else
   {err("not a sybase object",ptr);
    return(NULL);}}

LISP sybase_open(LISP args)
{int iflag;
 CS_RETCODE status;
 LISP value;
 setvar(sym_sybase_messages,NIL,NIL);
 iflag = no_interrupt(1);
 value = cons(NIL,NIL);
 value->storage_as.string.data = must_malloc(sizeof(struct sybase_state));
 memset(value->storage_as.string.data,0,sizeof(struct sybase_state));
 value->type = tc_sybase_state;
 setvar(sym_sybase_messages,NIL,NIL);
 status = sybase_setup(get_sybase_state(value,0),
		       getarg(args,"username","sa"),
		       getarg(args,"password",""),
		       getarg(args,"appname","htqx"));
 no_interrupt(iflag);
 if (status != CS_SUCCEED)
   return(err("sybase-open",statcons(status)));
 if NULLP(leval(sym_sybase,NIL))
   setvar(sym_sybase,value,NIL);
 return(value);}

LISP sybase_close(LISP value)
{int iflag;
 CS_RETCODE status;
 LISP obj;
 setvar(sym_sybase_messages,NIL,NIL);
 if NULLP(value)
   obj = leval(sym_sybase,NIL);
 else
   obj = value;
 iflag = no_interrupt(1);
 status = sybase_teardown(get_sybase_state(obj,1));
 free(obj->storage_as.string.data);
 obj->storage_as.string.data = NULL;
 if EQ(leval(sym_sybase,NIL),obj)
   setvar(sym_sybase,NIL,NIL);
 no_interrupt(iflag);
 return(statcons(status));}

LISP sybase_status(LISP value)
{int iflag;
 CS_RETCODE status;
 CS_INT mask,retlen;
 LISP result = NIL,obj;
 char buffer[256];
 struct sybase_state *sybase_state;
 setvar(sym_sybase_messages,NIL,NIL);
 if NULLP(value)
   obj = leval(sym_sybase,NIL);
 else
   obj = value;
 if (!(sybase_state = get_sybase_state(obj,0)))
   return(NIL);
 iflag = no_interrupt(1);
 status = ct_con_props(sybase_state->conn, CS_GET, CS_CON_STATUS,
		       &mask, 
		       CS_UNUSED,
		       NULL);
 if (status == CS_SUCCEED)
   result = cons(flocons(mask),result);
 status = ct_con_props(sybase_state->conn, CS_GET, CS_SERVERNAME,
		       buffer, 
		       sizeof(buffer),
		       &retlen);
 if (status == CS_SUCCEED)
   result = cons(strcons(retlen,buffer),result);
 status = ct_con_props(sybase_state->conn, CS_GET, CS_HOSTNAME,
		       buffer, 
		       sizeof(buffer),
		       &retlen);
 if (status == CS_SUCCEED)
   result = cons(strcons(retlen,buffer),result);
 no_interrupt(iflag);
 return(nreverse(result));}

struct col_data
{CS_SMALLINT indicator;
 void *value;
 CS_INT	valuelen;};

#define MAX_COL_RETURNS 50
static CS_DATAFMT *col_fmt = NULL;
static struct col_data *col_data = NULL;

/* todo: a ct_cancel if lisp error throws through fetch_data.
         Need book-keeping on what we have done to the cmd
         so that a sybase-cancel subr can be provided at user level
         for the above sort of thing */

CS_RETCODE CS_PUBLIC fetch_data(CS_COMMAND *cmd,LISP *header,LISP *items)
{CS_RETCODE retcode;
 CS_INT	num_cols,i,row_count = 0,rows_read = 0;
 CS_DATAFMT *fmt;
 struct col_data *data;
 LISP row;
 if (col_fmt == NULL)
   {col_fmt = (CS_DATAFMT *) malloc(MAX_COL_RETURNS * sizeof(CS_DATAFMT));
    memset(col_fmt,0,MAX_COL_RETURNS * sizeof(CS_DATAFMT));
    col_data = (struct col_data *) malloc(MAX_COL_RETURNS *
					  sizeof(struct col_data));
    memset(col_data,0,MAX_COL_RETURNS * sizeof(struct col_data));}
 retcode = ct_res_info(cmd, CS_NUMDATA, &num_cols, CS_UNUSED, NULL);
 if (retcode != CS_SUCCEED)
   {complain("fetch_data: ct_res_info() failed %d",retcode);
    return(retcode);}
 if (num_cols <= 0)
   {complain("fetch_data: ct_res_info() returned zero columns");
    return(CS_FAIL);}
 if (num_cols >= MAX_COL_RETURNS)
   {complain("fetch_data: ct_res_info() returned too many columns, %d",
	     num_cols);
    return(CS_FAIL);}
 if (siod_verbose_check(5))
   printf(";; %d columns\n",(int)num_cols);
 for (i = 0; i < num_cols; ++i)
   {data = &col_data[i];
    fmt = &col_fmt[i];
    retcode = ct_describe(cmd, (i + 1), fmt);
    if (retcode != CS_SUCCEED)
      {complain("fetch_data: ct_describe() failed %d",retcode);
       break;}
    if (data->value)
      {free(data->value);
       data->value = NULL;}
    if (siod_verbose_check(5))
      printf(";; %d %.*s %s[%d] %d\n",(int)i,(int)fmt->namelen,fmt->name,
	     (sybase_typestr(fmt->datatype))
	     ? sybase_typestr(fmt->datatype) : "?",
	     (int)fmt->maxlength,
	     (int)fmt->usertype);
    switch(fmt->datatype)
      {case CS_TINYINT_TYPE:
       case CS_SMALLINT_TYPE: 
       case CS_INT_TYPE:
       case CS_LONG_TYPE:
       case CS_REAL_TYPE:
       case CS_FLOAT_TYPE:
       case CS_BIT_TYPE:
	 fmt->datatype = CS_FLOAT_TYPE;
	 data->valuelen = sizeof(double);
	 break;
       case CS_CHAR_TYPE:
       case CS_VARCHAR_TYPE:
       case CS_TEXT_TYPE:
	 fmt->datatype = CS_CHAR_TYPE;
	 fmt->format = CS_FMT_NULLTERM;
	 fmt->maxlength = fmt->maxlength + 1;
	 data->valuelen = fmt->maxlength;
	 break;
       case CS_BINARY_TYPE:
       case CS_VARBINARY_TYPE:
	 fmt->datatype = CS_CHAR_TYPE;
	 fmt->format = CS_FMT_NULLTERM;
	 fmt->maxlength = (2 * fmt->maxlength) + 3;
	 data->valuelen = fmt->maxlength;
	 break;
       case CS_MONEY_TYPE:
       case CS_MONEY4_TYPE:
	 fmt->datatype = CS_CHAR_TYPE;
	 fmt->format = CS_FMT_NULLTERM;
	 fmt->maxlength = 25;
	 data->valuelen = fmt->maxlength;
	 break;
       case CS_DATETIME_TYPE:
       case CS_DATETIME4_TYPE:
	 fmt->datatype = CS_CHAR_TYPE;
	 fmt->format = CS_FMT_NULLTERM;
	 fmt->maxlength = 31;
	 data->valuelen = fmt->maxlength;
	 break;
       case CS_NUMERIC_TYPE:
       case CS_DECIMAL_TYPE:
	 fmt->datatype = CS_CHAR_TYPE;
	 fmt->format = CS_FMT_NULLTERM;
	 fmt->maxlength = CS_MAX_PREC + 3;
	 data->valuelen = fmt->maxlength;
	 break;
       case CS_IMAGE_TYPE:
	 data->valuelen = fmt->maxlength;
	 break;
       default:
	 fmt->datatype = CS_CHAR_TYPE;
	 fmt->format = CS_FMT_NULLTERM;
	 fmt->maxlength = 30;
	 data->valuelen = fmt->maxlength;
	 break;}
    data->value = malloc(data->valuelen);
    retcode = ct_bind(cmd, (i + 1), fmt,
		      data->value, &data->valuelen,
		      &data->indicator);
    if (retcode != CS_SUCCEED)
      {complain("fetch_data: ct_bind() failed %d",retcode);
       return(retcode);}}
 *header = arcons(tc_lisp_array,num_cols,0);
 for(i = 0; i < num_cols; ++i)
   {fmt = &col_fmt[i];
    (*header)->storage_as.lisp_array.data[i] =
      (*fmt->name) ? rintern(fmt->name) : cintern("NONAME");}
 *items = NIL;
 while (((retcode = ct_fetch(cmd, CS_UNUSED, CS_UNUSED, CS_UNUSED,
			     &rows_read)) == CS_SUCCEED) ||
	(retcode == CS_ROW_FAIL))
   {row_count = row_count + rows_read;
    if (rows_read > 1)
      /* since fmt->count should be = 0, we will only see
	 one row fetched at a time. Otherwise this code has no
	 hope of working */
      complain("suprised that rows_read = %d",rows_read);
    if (retcode == CS_ROW_FAIL)
      complain("Error on row %d.", row_count);
    row = arcons(tc_lisp_array,num_cols,0);
    *items = cons(row,*items);
    for (i = 0; i < num_cols; i++)
      {data = &col_data[i];
       fmt = &col_fmt[i];
       if (data->indicator >= 0)
	 switch(fmt->datatype)
	   {case CS_CHAR_TYPE:
	      row->storage_as.lisp_array.data[i] =
		strcons(strlen((char *)data->value),
			(char *) data->value);
	      break;
	    case CS_FLOAT_TYPE:
	      row->storage_as.lisp_array.data[i] =
		flocons(*((double *) data->value));
	      break;
	    case CS_IMAGE_TYPE:
	      row->storage_as.lisp_array.data[i] =
		arcons(tc_byte_array,fmt->maxlength,0);
	      memcpy(row->storage_as.lisp_array.data[i]->storage_as.string.data,
		     data->value,
		     fmt->maxlength);
	      break;
	    default:
	      errswitch();}}}
 *items = nreverse(*items);
 switch (retcode)
   {case CS_END_DATA:
      retcode = CS_SUCCEED;
      break;
    case CS_FAIL:
      complain("fetch_data: ct_fetch() failed, %d rows",rows_read);
      return(retcode);
    default:
      complain("fetch_data: ct_fetch() returned an expected retcode %d",
	       retcode);}
 return(retcode);}

#define MAX_PARAM_ARGS 50

/* these are being allocated on the heap as an array because I don't
    know if ct_param holds on to pointers to things in some internal
    structure waiting for results to be sent.
    Note that we are passing pointers to allocated lisp data
    so that the GC should be told to not release or move certain
    things. If ct_param does do immediate copy then the above is moot.
    But these issues are not covered in the Sybase documentation.
 */

static CS_DATAFMT *param_fmt = NULL;
static CS_INT *param_data = NULL;

LISP sybase_execute(LISP arglist)
{CS_RETCODE status = CS_SUCCEED;
 LISP obj,str,kind,args;
 char *cmdstr,*argname;
 int iflag,j,m;
 CS_INT restype,ivalue;
 LISP l,argvalue;
 CS_VOID *data;
 CS_INT datalen;
 CS_SMALLINT msg_id;
 LISP header,items,result = NIL;
 struct sybase_state *sybase_state;
 setvar(sym_sybase_messages,NIL,NIL);
 l = arglist;
 obj = poparg(&l,NIL);
 if TYPEP(obj,tc_sybase_state)
   str = poparg(&l,NIL);
 else
   {str = obj;
    obj = leval(sym_sybase,NIL);}
 kind = poparg(&l,NIL);
 args = l;
 cmdstr = get_c_string(str);
 sybase_state = get_sybase_state(obj,1);
 iflag = no_interrupt(1);
 if (!param_fmt)
   {param_fmt = (CS_DATAFMT *) malloc(sizeof(CS_DATAFMT) * MAX_PARAM_ARGS);
    param_data = (CS_INT *) malloc(sizeof(CS_INT) * MAX_PARAM_ARGS);}
 status = ct_command(sybase_state->cmd,
		     NNULLP(kind) ? get_c_long(kind) : CS_LANG_CMD,
		     cmdstr,
		     CS_NULLTERM,
		     CS_UNUSED);
 if (status != CS_SUCCEED)
   {complain("ct_command %s %d",cmdstr,status);
    no_interrupt(iflag);
    return(statcons(status));}
 for(j=0,l=args;NNULLP(l);l=cddr(l))
   {if (j == MAX_PARAM_ARGS)
      err("too many arguments for sybase-execute to handle",args);
    memset(&param_fmt[j],0,sizeof(param_fmt[j]));
    argname = get_c_string(car(l));
    argvalue = cadr(l);
    m = strlen(argname);
    if (m >= (CS_MAX_NAME - 1)) err("argument name too long",caar(l));
    strcpy(param_fmt[j].name, argname);
    param_fmt[j].namelen = CS_NULLTERM;
    param_fmt[j].status = CS_INPUTVALUE;
    switch(TYPE(argvalue))
      {case tc_nil:
	 data = NULL;
	 datalen = 0;
	 param_fmt[j].datatype = CS_CHAR_TYPE;
	 param_fmt[j].maxlength = 0;
	 break;
       case tc_flonum:
	 ivalue = argvalue->storage_as.flonum.data;
	 if (ivalue == argvalue->storage_as.flonum.data)
	   /* there is not always automatic conversion of parameters,
	      hence we convert when it looks like we can.
	      In general we could support argvalue as a PAIR,
	      with the car being the datatype we want, and the CADR
	      being the data */
	   {param_data[j] = ivalue;
	    data = &param_data[j];
	    datalen = sizeof(param_data[j]);
	    param_fmt[j].datatype = CS_INT_TYPE;
	    param_fmt[j].maxlength = datalen;}
	 else
	   {data = &argvalue->storage_as.flonum.data;
	    datalen = sizeof(argvalue->storage_as.flonum.data);
	    param_fmt[j].datatype = CS_FLOAT_TYPE;
	    param_fmt[j].maxlength = datalen;}
	 break;
       case tc_symbol:
       case tc_string:
	 data = get_c_string(argvalue);
	 datalen = strlen(data);
	 param_fmt[j].datatype = CS_CHAR_TYPE;
	 param_fmt[j].maxlength = datalen;
	 break;
       case tc_long_array:
	 data = argvalue->storage_as.long_array.data;
	 datalen = argvalue->storage_as.long_array.dim * sizeof(long);
	 param_fmt[j].datatype = CS_BINARY_TYPE;
	 param_fmt[j].maxlength = datalen;
	 break;
       case tc_byte_array:
	 data = argvalue->storage_as.string.data;
	 datalen = argvalue->storage_as.string.dim;
	 param_fmt[j].datatype = CS_BINARY_TYPE;
	 param_fmt[j].maxlength = datalen;
	 break;
       default:
	 return(err("sybase-execute can't handle data type",argvalue));}
    status = ct_param(sybase_state->cmd,
		      &param_fmt[j],data,datalen,
		      CS_UNUSED);
    if (status != CS_SUCCEED)
      {complain("sybase-execute ct_param %d",status);
       no_interrupt(iflag);
       return(statcons(status));}}
 status = ct_send(sybase_state->cmd);
 if (status != CS_SUCCEED)
   {complain("ct_send %s %d",cmdstr,status);
    no_interrupt(iflag);
    return(statcons(status));}
 while ((status = ct_results(sybase_state->cmd, &restype)) == CS_SUCCEED)
   switch (restype)
     {case CS_ROW_RESULT:
      case CS_PARAM_RESULT:
      case CS_STATUS_RESULT:
	result = cons(cons(cintern((restype == CS_ROW_RESULT)
				   ? "CS_ROW_RESULT" :
				   (restype == CS_PARAM_RESULT)
				   ? "CS_PARAM_RESULT" :
				   "CS_STATUS_RESULT"),
			   NIL),
		      result);
	status = fetch_data(sybase_state->cmd,&header,&items);
	if (status != CS_SUCCEED)
	  {complain("fetch_data() failed");
	   ct_cancel(NULL, sybase_state->cmd, CS_CANCEL_ALL);
	   no_interrupt(iflag);
	   return(flocons(status));}
	setcdr(car(result),cons(header,items));
	break;
      case CS_MSG_RESULT:
	status = ct_res_info(sybase_state->cmd, CS_MSGTYPE,
			     &msg_id, CS_UNUSED, NULL);
	if (status != CS_SUCCEED)
	  {complain("ct_res_info(msgtype) failed");
	   ct_cancel(NULL, sybase_state->cmd, CS_CANCEL_ALL);
	   no_interrupt(iflag);
	   return(flocons(status));}
	result = cons(listn(2,
			    cintern("CS_MSG_RESULT"),
			    flocons(msg_id)),
		      result);
	break;
      case CS_CMD_SUCCEED:
	break;
      case CS_CMD_DONE:
	break;
      case CS_CMD_FAIL:
	complain("ct_results CS_CMD_FAIL");
	result = cons(cintern("CS_CMD_FAIL"),result);
	break;
      default:
	complain("ct_results unexpected type: %d",restype);
	result = cons(cintern("CS_CMD_???"),result);
	ct_cancel(NULL, sybase_state->cmd, CS_CANCEL_ALL);}
 switch(status)
   {case CS_END_RESULTS:
      status = CS_SUCCEED;
      break;
    case CS_FAIL:
      complain("ct_results CS_FAIL");
      break;
    default:
      complain("ct_results unexpected status %d",status);
      break;}
 no_interrupt(iflag);
 return(cons(statcons(status),nreverse(result)));}

static long ignore_msgs[] = {5701,5703,5704};

void push_sybase_messages(LISP value)
{setvar(sym_sybase_messages,
	cons(value,leval(sym_sybase_messages,NIL)),
	NIL);}


static CS_RETCODE server_err_handler(CS_CONTEXT *cp,
				     CS_CONNECTION *chp,
				     CS_SERVERMSG *msgp)
{long j,docomplain = 1;
 for(j=0;j<(sizeof(ignore_msgs)/sizeof(ignore_msgs[0]));++j)
   if (msgp->msgnumber == ignore_msgs[j])
     {docomplain = 0;
      break;}
 if (docomplain)
   complain("Server: msgno(%d) severity(%d) state(%d)\n%s",
	    msgp->msgnumber, msgp->severity, msgp->state,
	    msgp->text);
 (push_sybase_messages
  (listn(5,
	 cintern("server"),
	 cons(cintern("msgnumber"),flocons(msgp->msgnumber)),
	 cons(cintern("severity"),flocons(msgp->severity)),
	 cons(cintern("state"),flocons(msgp->state)),
	 cons(cintern("text"),strcons(strlen(msgp->text),msgp->text)))));
 return(CS_SUCCEED);}

static CS_RETCODE client_err_handler(CS_CONTEXT *cp,
				     CS_CONNECTION *chp,
				     CS_CLIENTMSG *emsgp)
{LISP note;
 complain("Client: error(%d) serverity(%d) OS error(%d)\n%s",
	  emsgp->msgnumber, emsgp->severity, emsgp->osnumber,
	  emsgp->msgstring);
 if ((emsgp->osstringlen != 0) && (emsgp->osstringlen != CS_UNUSED))
   complain("%s",emsgp->osstring);
 note = listn(5,
	      cintern("client"),
	      cons(cintern("msgnumber"),flocons(emsgp->msgnumber)),
	      cons(cintern("severity"),flocons(emsgp->severity)),
	      cons(cintern("osnumber"),flocons(emsgp->osnumber)),
	      cons(cintern("msgstring"),
		   strcons(strlen(emsgp->msgstring),emsgp->msgstring)));
 if ((emsgp->osstringlen != 0) && (emsgp->osstringlen != CS_UNUSED))
   note = nconc(note,
		cons(cons(cintern("osstring"),
			  strcons(emsgp->osstringlen,emsgp->osstring)),
		     NIL));
 push_sybase_messages(note);
 return (CS_SUCCEED);}

static CS_RETCODE cs_err_handler(CS_CONTEXT *cp,CS_CLIENTMSG *msg)
{LISP note;
 complain("CS-Library error %ld/%ld/%ld/%ld - %s",
	  CS_LAYER(msg->msgnumber), 
	  CS_ORIGIN(msg->msgnumber), 
	  CS_SEVERITY(msg->msgnumber), 
	  CS_NUMBER(msg->msgnumber), 
	  msg->msgstring);
 if (msg->osstringlen > 0)
   complain("CS-Library OS error %ld - %s.",
	    msg->osnumber, msg->osstring);
 note = listn(6,
	      cintern("cs-library"),
	      cons(cintern("layer"),flocons(CS_LAYER(msg->msgnumber))),
	      cons(cintern("origin"),flocons(CS_ORIGIN(msg->msgnumber))),
	      cons(cintern("severity"),flocons(CS_SEVERITY(msg->msgnumber))),
	      cons(cintern("number"),flocons(CS_NUMBER(msg->msgnumber))),
	      cons(cintern("msgstring"),
		   strcons(strlen(msg->msgstring),msg->msgstring)));
 if (msg->osstringlen > 0)
   note = nconc(note,
		cons(cons(cintern("osstring"),
			  strcons(msg->osstringlen,msg->osstring)),
		     NIL));
 push_sybase_messages(note);
 return (CS_SUCCEED);}

void sybase_prin1(LISP ptr,struct gen_printio *f)
{char buff[256];
 sprintf(buff,"#<SYBASE %p>",ptr->storage_as.string.data);
 gput_st(f,buff);}

void sybase_gc_free(LISP ptr)
{struct sybase_state *sybase_state;
 if ((sybase_state = (struct sybase_state *) ptr->storage_as.string.data))
   {sybase_teardown(sybase_state);
    free(ptr->storage_as.string.data);
    ptr->storage_as.string.data = NULL;}}

void init_sql_sybase(void)
{long j;
 tc_sybase_state = allocate_user_tc();
 set_gc_hooks(tc_sybase_state,
	      NULL,
	      NULL,
	      NULL,
	      sybase_gc_free,
	      &j);
 set_print_hooks(tc_sybase_state,sybase_prin1);
 init_lsubr("sybase-open",sybase_open);
 init_subr_1("sybase-close",sybase_close);
 init_subr_1("sybase-status",sybase_status);
 init_lsubr("sybase-execute",sybase_execute);
 init_sql_sybasec();
 gc_protect_sym(&sym_sybase,"*sybase*");
 setvar(sym_sybase,NIL,NIL);
 gc_protect_sym(&sym_sybase_messages,"*sybase-messages*");
 setvar(sym_sybase_messages,NIL,NIL);
 init_sql_sybase_version();}

