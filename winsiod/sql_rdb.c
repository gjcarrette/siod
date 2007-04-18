/*****************************************************************************/
/**                                                                         **/
/**  Copyright (C) 1992-1994 BY                                             **/
/**  MITECH CORPORATION, ACTON, MASSACHUSETTS.                              **/
/**  ALL RIGHTS RESERVED.                                                   **/
/**                                                                         **/
/** Permission to use, copy, modify, distribute and sell this software      **/
/** and its documentation for any purpose and without fee is hereby         **/
/** granted, provided that the above copyright notice appear in all copies  **/
/** and that both that copyright notice and this permission notice appear   **/
/** in supporting documentation, and that the name of Mitech Corporation    **/
/** not be used in advertising or publicity pertaining to distribution      **/
/** of the software without specific, written prior permission.             **/
/**                                                                         **/
/** MITECH DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING **/
/** ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL**/
/** MITECH BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR  **/
/** ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,     **/
/** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  **/
/** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS     **/
/** SOFTWARE.                                                               **/
/**                                                                         **/
/*****************************************************************************/

/* Interface from SIOD to DIGITAL RDB SQL SERVICES.
   20-JAN-94 George Carrette. GJC@MITECH.COM
   This is a good demonstration of the fact that a direct
   argument-for-argument translation of a C-language API into a lisp
   language API is inferior to the more studied approach used here,
   where we can eliminate the need for redundant arguments by encapsulation
   of state.

Building: With SIOD 3.0, under VMS use $MMS/MACRO=("EXTRA=sql_rdb")
          which compiles siod.c with #define INIT_EXTRA init_sql_rdb
          and add sql_rdb.obj to the link statement.
          Or use $@MAKEFILE EXTRA SQL_RDB
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlsrv.h>
#include <math.h>

#include "siod.h"

static void init_sql_rdb_version(void)
{setvar(cintern("*sql_rdb-version*"),
	cintern("$Id: sql_rdb.c,v 1.1 1996/10/17 18:40:18 gjc Exp $"),
	NIL);}

LISP sym_ascii_string = NIL;
LISP sym_generalized_number = NIL;
LISP sym_generalized_date = NIL;
LISP sym_varchar = NIL;
LISP sym_varbyte = NIL;
LISP sym_list_varbyte = NIL;
LISP sym_table = NIL;
LISP sym_list = NIL;
LISP sym_update = NIL;
LISP sym_read_only = NIL;
LISP sym_insert_only = NIL;
LISP sym_begin = NIL;
LISP sym_end = NIL;
LISP sym_abort = NIL;

LISP associations = NIL;

long cursor_gensym_counter = 0;

static long tc_extra = 0;
#define extra_tc_association 1
#define extra_tc_statement 2

struct association
{LISP args;
 LISP statements;
 ASSOCIATE_ID id;
 char *error_buffer;
 char *read_buffer;
 char *write_buffer;
 long error_buffer_size;
 long read_buffer_size;
 long write_buffer_size;};

struct statement
{LISP association;
 LISP param_alist;
 LISP select_alist;
 LISP cursor;
 long id;
 SQLDA_ID params;
 SQLDA_ID selects;};

LISP list3(LISP a,LISP b,LISP c)
{return(cons(a,cons(b,cons(c,NIL))));}

void ps_i(char *fmt,long n)
{char buff[512];
 sprintf(buff,fmt,n);
 put_st(buff);}

static LISP extcons(long length,long typec)
{long flag;
 LISP s;
 flag = no_interrupt(1);
 s = cons(NIL,NIL);
 s->type = tc_extra;
 s->storage_as.string.data = must_malloc(length);
 s->storage_as.string.dim = typec;
 memset(s->storage_as.string.data,0,length);
 no_interrupt(flag);
 return(s);}

LISP sqlrtl_associate(LISP l)
{LISP obj,tmp;
 long iflag,local_flag,status;
 struct ASSOCIATE_STR  associate_str;
 struct association *a;
 char *nodename,*username,*password,*class_name;
 long majerr,suberr1,suberr2;
 char errmsg[512],*estatus;
 memset(&associate_str,0,sizeof(associate_str));
 if NNULLP(cadr(assq(cintern("log"),l)))
   {associate_str.CLIENT_LOG = SQLSRV_LOG_ASSOCIATION + SQLSRV_LOG_ROUTINE;
    associate_str.SERVER_LOG = SQLSRV_LOG_ASSOCIATION;}
 iflag = no_interrupt(1);
 tmp = cadr(assq(cintern("node"),l));
 if NULLP(tmp)
   {local_flag = 1;
    nodename = "0";}
 else
   {local_flag = 0;
    nodename = get_c_string(tmp);}
 username = NNULLP(tmp = cadr(assq(cintern("username"),l)))
   ? get_c_string(tmp) : NULL;
 password = NNULLP(tmp = cadr(assq(cintern("password"),l)))
   ? get_c_string(tmp) : 0;
 tmp = cadr(assq(cintern("class"),l));
 class_name = NNULLP(tmp) ? get_c_string(tmp) : NULL;
 if (class_name)
   {status = sqlsrv_set_server_class(class_name);
    if (status != SQL_SUCCESS)
      {sprintf(errmsg,"sqlsrv_set_server_class error %d",status);
       return(err(errmsg,NIL));}}
 obj = extcons(sizeof(struct association),extra_tc_association);
 a = (struct association *) obj->storage_as.string.data;
 a->args = l;
 a->statements = NIL;
 a->id = 0;
 a->error_buffer_size = 512;
 a->read_buffer_size = 1024;
 a->write_buffer_size = 1024;
 a->error_buffer = (char *) malloc(a->error_buffer_size);
 a->read_buffer = (char *) malloc(a->read_buffer_size);
 a->write_buffer = (char *) malloc(a->write_buffer_size);
 associate_str.ERRBUFLEN = a->error_buffer_size;
 associate_str.ERRBUF = (unsigned char *)a->error_buffer;
 associate_str.LOCAL_FLAG = local_flag;
 associate_str.MEMORY_ROUTINE = NULL;
 associate_str.FREE_MEMORY_ROUTINE = NULL;
 status = sqlsrv_associate(nodename,
			   username,
			   password,
			   (unsigned char *)a->read_buffer,
			   (unsigned char *)a->write_buffer,
			   a->read_buffer_size,
			   a->write_buffer_size,
			   0,
			   &associate_str,
			   &a->id);
 if (status == SQL_SUCCESS)
   {associations = cons(obj,associations);
    no_interrupt(iflag);
    return(obj);}
 if (a->id)
   /* The id may be set even though the status is not success.
      This area of the API is not well documented. */
   {sqlsrv_sqlca_error(a->id,&majerr,&suberr1,&suberr2);
    sqlsrv_release(a->id,0);}
 else
   {suberr1 = 0;
    suberr2 = 0;
    free(a->error_buffer);
    free(a->read_buffer);
    free(a->write_buffer);}
 sprintf(errmsg,"sqlsrv_associate error %d, %d, %d",
	 status,suberr1,suberr2);
 return(err(errmsg,NIL));}

struct association *get_association(LISP assoc,long oflag)
{struct association *a;
 if ((TYPE(assoc) != tc_extra) ||
     (assoc->storage_as.string.dim != extra_tc_association))
   return(err("not an association",assoc));
 a = (struct association *)assoc->storage_as.string.data;
 if (a->id || !oflag)
   return(a);
 else
   {err("sqlsrv association has been released",assoc);
    return(NULL);}}

LISP sqlrtl_associations(void)
{return(associations);}

LISP sqlrtl_release(LISP assoc)
{struct association *a;
 long iflag,status;
 char errmsg[512];
 a = get_association(assoc,1);
 iflag = no_interrupt(1);
 status = sqlsrv_release(a->id,0);
 free(a->error_buffer);
 free(a->read_buffer);
 free(a->write_buffer);
 memset(a,0,sizeof(struct association));
 associations = delq(assoc,associations);
 if (status != SQL_SUCCESS)
   {sprintf(errmsg,"sqlsrv_release error %d",status);
    err(errmsg,NIL);}
 no_interrupt(iflag);
 return(nullp(NIL));}

LISP sqlrtl_error(ASSOCIATE_ID id)
{long majerr,suberr1,suberr2;
 char *estatus,errmsg[512];
 sqlsrv_sqlca_error(id,&majerr,&suberr1,&suberr2);
 sprintf(errmsg,"SQL error %d %d %d",majerr,suberr1,suberr2);
 return(err(errmsg,NIL));}

LISP sqlrtl_status_error(long status)
{char errmsg[512];
 sprintf(errmsg,"SQL error %d",status);
 return(err(errmsg,NIL));}

LISP sqlrtl_sqlca_num_batch_rows(LISP assoc)
{long status,iflag;
 struct association *a;
 a = get_association(assoc,1);
 iflag = no_interrupt(1);
 status = sqlsrv_sqlca_num_batch_rows(a->id);
 if (status >= 0)
   {no_interrupt(iflag);
    return(flocons(status));}
 else
   return(sqlrtl_error(a->id));}

LISP sqlrtl_sqlca_count(LISP assoc)
{long status,iflag;
 struct association *a;
 a = get_association(assoc,1);
 iflag = no_interrupt(1);
 status = sqlsrv_sqlca_count(a->id);
 if (status >= 0)
   {no_interrupt(iflag);
    return(flocons(status));}
 else
   return(sqlrtl_error(a->id));}
 
 
LISP sqlrtl_execute_immediate(LISP assoc,LISP stmt)
{long status,iflag;
 char *st;
 struct association *a;
 a = get_association(assoc,1);
 st = get_c_string(stmt);
 iflag = no_interrupt(1);
 status = sqlsrv_execute_immediate(a->id,0,st);
 if (status == SQL_SUCCESS)
   {no_interrupt(iflag);
    return(nullp(NIL));}
 else
   return(sqlrtl_error(a->id));}

LISP sqlrtl_describe_association(LISP assoc)
{struct association *a;
 a = get_association(assoc,0);
 put_st("An SQLSRV association\n");
 put_st("Opened with: ");
 lprin1f(a->args,stdout);
 put_st("\n");
 ps_i("Statements: %d\n",get_c_long(llength(a->statements)));
 if (a->id)
   {ps_i("Associate id: %x\n",a->id);
    put_st("error buffer: ");
    put_st(a->error_buffer);
    put_st("\n");}
 else
   put_st("which has been released\n");
 return(NIL);}

LISP sqlrtl_sqlda_alist(SQLDA_ID x)
{long i,n;
 char name[1000];
 LISP vtype,l;
 short namelen,type,scale;
 unsigned short len;
 n = sqlsrv_sqlda_sqld(x);
 l = NIL;
 for(i = 0; i < n; ++i)
   {sqlsrv_sqlda_column_name(x,i,&name[0],&namelen);
    sqlsrv_sqlda_column_type(x,i,&type,&len,&scale,0);
    switch(type)
      {case SQLSRV_ASCII_STRING: vtype = sym_ascii_string; break;
       case SQLSRV_GENERALIZED_NUMBER: vtype = sym_generalized_number; break;
       case SQLSRV_GENERALIZED_DATE: vtype = sym_generalized_date; break;
       case SQLSRV_VARCHAR: vtype = sym_varchar; break;
       case SQLSRV_VARBYTE: vtype = sym_varbyte; break;
       case SQLSRV_LIST_VARBYTE: vtype = sym_list_varbyte; break;
       default: vtype = NIL; break;}
    name[namelen] = 0;
    l = cons(list3(rintern(name),flocons(i),vtype),l);}
 return(nreverse(l));}

#define SQL_WSR_CHARSET "\t\n"

LISP sqlrtl_prepare(LISP assoc,LISP sql)
{long iflag,status,tmpn;
 char *s,cname[32],*tmps = NULL,*ptr;
 struct statement *c;
 struct association *a;
 LISP st;
 a = get_association(assoc,1);
 s = get_c_string(sql);
 iflag = no_interrupt(1);
 st = extcons(sizeof(struct statement),extra_tc_statement);
 c = (struct statement *) st->storage_as.string.data;
 c->association = assoc;
 tmpn = strlen(s);
 if (tmpn != strcspn(s,SQL_WSR_CHARSET))
   {tmps = (char *) malloc(tmpn+1);
    strcpy(tmps,s);
    for(ptr=tmps;*ptr;++ptr) if (strchr(SQL_WSR_CHARSET,*ptr)) *ptr = ' ';
    s = tmps;}
 status = sqlsrv_prepare(a->id,0,s,&c->id,&c->params,&c->selects);
 if (tmps) free(tmps);
 if (status != SQL_SUCCESS)
   sqlrtl_error(a->id);
 if (c->params)
   {status = sqlsrv_allocate_sqlda_data(a->id,c->params);
    if (status != SQL_SUCCESS)
      {sqlsrv_release_statement(a->id,1,&c->id);
       sqlrtl_error(a->id);}}
 if (c->selects)
   {status = sqlsrv_allocate_sqlda_data(a->id,c->selects);
    if (status != SQL_SUCCESS)
      {sqlsrv_release_statement(a->id,1,&c->id);
       sqlrtl_error(a->id);}}
 c->param_alist = NIL;
 c->select_alist = NIL;
 c->cursor = NIL;
 a->statements = cons(st,a->statements);
 c->param_alist = sqlrtl_sqlda_alist(c->params);
 c->select_alist = sqlrtl_sqlda_alist(c->selects);
 if (c->selects)
   {sprintf(cname,"CUR_%06D",++cursor_gensym_counter);
    c->cursor = strcons(strlen(cname),cname);}
 no_interrupt(iflag);
 return(st);}

struct statement *get_statement(LISP st)
{if ((TYPE(st) != tc_extra) ||
     (st->storage_as.string.dim != extra_tc_statement))
   {err("not a statement",st);
    return(NULL);}
 else
   return((struct statement *)st->storage_as.string.data);}

LISP sqlrtl_statement_params(LISP x)
{return((get_statement(x))->param_alist);}

LISP sqlrtl_statement_selects(LISP x)
{return((get_statement(x))->select_alist);}

LISP sqlrtl_statement_association(LISP x)
{return((get_statement(x))->association);}

void describe_sqlda(SQLDA_ID x)
{long i,n;
 char name[100],*vtype;
 short namelen,type,scale;
 unsigned short len;
 n = sqlsrv_sqlda_sqld(x);
 ps_i("sqlda: %d elements\n",n);
 for(i = 0; i < n; ++i)
   {sqlsrv_sqlda_column_name(x,i,&name[0],&namelen);
    sqlsrv_sqlda_column_type(x,i,&type,&len,&scale,0);
    switch(type)
      {case SQLSRV_ASCII_STRING: vtype = "ASCII_STRING"; break;
       case SQLSRV_GENERALIZED_NUMBER: vtype = "GENERALIZED_NUMBER"; break;
       case SQLSRV_GENERALIZED_DATE: vtype = "GENERALIZED_DATE"; break;
       case SQLSRV_VARCHAR: vtype = "VARCHAR"; break;
       case SQLSRV_VARBYTE: vtype = "VARBYTE"; break;
       case SQLSRV_LIST_VARBYTE: vtype = "LIST_VARBYTE"; break;
       default: vtype = "????"; break;}
    name[namelen] = 0;
    put_st(name);
    put_st(": ");
    put_st(vtype);
    ps_i(" %d \n",len);}}

LISP sqlrtl_describe_statement(LISP x)
{struct statement *c;
 c = get_statement(x);
 put_st("A prepared SQL statement\n");
 ps_i("ID: %d\n",c->id);
 if (c->params)
   {put_st("param ");
    describe_sqlda(c->params);}
 if (c->selects)
   {put_st("select ");
    describe_sqlda(c->selects);
    put_st("Cursor: ");
    lprin1f(c->cursor,stdout);
    put_st("\n");}
 return(NIL);}

LISP sqlrtl_release_statement(LISP x)
{struct statement *c;
 struct association *a;
 LISP assoc;
 long status,iflag;
 c = get_statement(x);
 assoc = c->association;
 a = get_association(assoc,1);
 iflag = no_interrupt(1);
 status = sqlsrv_release_statement(a->id,1,&c->id);
 a->statements = delq(x,a->statements);
 if (status != SQL_SUCCESS)
   sqlrtl_error(a->id);
 no_interrupt(iflag);
 return(NIL);}

LISP sqlrtl_declare_cursor(LISP stmt,LISP type,LISP mode)
{long status,iflag,itype,imode;
 struct statement *c;
 struct association *a;
 LISP cursor;
 c = (struct statement *)get_statement(stmt);
 a = get_association(c->association,1);
 cursor = c->cursor;
 if NULLP(cursor)
   return(err("statement has no cursor",stmt));
 if (NULLP(type) && NULLP(mode))
   return(NIL);
 else
   {if EQ(type,sym_table)
      itype = SQLSRV_TABLE_CURSOR;
    else if EQ(type,sym_list)
      itype = SQLSRV_LIST_CURSOR;
    else
      err("invalid cursor type",type);
    if EQ(mode,sym_update)
      imode = SQLSRV_MODE_UPDATE;
    else if EQ(mode,sym_read_only)
      imode = SQLSRV_MODE_READ_ONLY;
    else if EQ(mode,sym_insert_only)
      imode = SQLSRV_MODE_INSERT_ONLY;
    else
      err("invalid cursor mode",mode);}
 iflag = no_interrupt(1);
 status = sqlsrv_declare_cursor(a->id,get_c_string(cursor),c->id,itype,imode);
 if (status == SQL_SUCCESS)
   {no_interrupt(iflag);
    return(nullp(NIL));}
 return(sqlrtl_error(a->id));}

LISP sqlrtl_execute(LISP stmt,LISP batchp)
{struct statement *c;
 struct association *a;
 long status,iflag,eflag;
 c = get_statement(stmt);
 a = get_association(c->association,1);
 if NULLP(batchp)
   eflag = 0;
 else if EQ(batchp,sym_begin)
   eflag = 1;
 else if EQ(batchp,sym_end)
   eflag = 2;
 else if EQ(batchp,sym_abort)
   eflag = 3;
 else
   err("invalid batch execute mode",batchp);
 iflag = no_interrupt(1);
 status = sqlsrv_execute(a->id,0,c->id,eflag,c->params);
 if (status == SQL_SUCCESS)
   {no_interrupt(iflag);
    return(nullp(NIL));}
 sqlrtl_error(a->id);}

LISP sqlrtl_open_cursor(LISP stmt)
{struct statement *c;
 struct association *a;
 long status,iflag;
 LISP cursor;
 c = get_statement(stmt);
 a = get_association(c->association,1);
 cursor = c->cursor;
 if NULLP(cursor)
   err("statement has no cursor",stmt);
 iflag = no_interrupt(1);
 status = sqlsrv_open_cursor(a->id,get_c_string(cursor),c->id,c->params);
 if (status == SQL_SUCCESS)
   {no_interrupt(iflag);
    return(nullp(NIL));}
 sqlrtl_error(a->id);}

LISP sqlrtl_fetch(LISP stmt)
{struct statement *c;
 struct association *a;
 long status,iflag;
 LISP cursor;
 c = get_statement(stmt);
 a = get_association(c->association,1);
 cursor = c->cursor;
 if NULLP(cursor)
   err("statement has no cursor",stmt);
 iflag = no_interrupt(1);
 status = sqlsrv_fetch(a->id,get_c_string(cursor),0,0,c->selects);
 switch(status)
   {case SQL_SUCCESS:
    case 1:
      /* with fetch_many in use this sometimes returned 1 */
      no_interrupt(iflag);
      return(nullp(NIL));
    case SQL_EOS:
      no_interrupt(iflag);
      return(NIL);
    default:
      sqlrtl_error(a->id);}}

LISP sqlrtl_fetch_many(LISP stmt,LISP count)
{struct statement *c;
 struct association *a;
 long status,iflag,k;
 LISP cursor;
 c = get_statement(stmt);
 a = get_association(c->association,1);
 cursor = c->cursor;
 if NULLP(cursor)
   err("statement has no cursor",stmt);
 if NULLP(count)
   k = 0;
 else
   k = get_c_long(count);
 iflag = no_interrupt(1);
 status = sqlsrv_fetch_many(a->id,get_c_string(cursor),0,k);
 switch(status)
   {case SQL_SUCCESS:
      no_interrupt(iflag);
      return(nullp(NIL));
    case SQL_EOS:
      no_interrupt(iflag);
      return(NIL);
    default:
      sqlrtl_error(a->id);}}

LISP sqlrtl_close_cursor(LISP stmt)
{struct statement *c;
 struct association *a;
 long status,iflag;
 LISP cursor;
 c = get_statement(stmt);
 a = get_association(c->association,1);
 cursor = c->cursor;
 if NULLP(cursor)
   err("statement has no cursor",stmt);
 iflag = no_interrupt(1);
 status = sqlsrv_close_cursor(a->id,get_c_string(cursor));
 if (status == SQL_SUCCESS)
   {no_interrupt(iflag);
    return(nullp(NIL));}
 sqlrtl_error(a->id);}

LISP sqlrtl_get_datum(SQLDA_ID x,long k)
{short typ,scl,nullp,varlen;
 unsigned short len;
 char *data;
 long status,iflag;
 LISP result;
 double d;
 iflag = no_interrupt(1);
 if (k < sqlsrv_sqlda_sqld(x))
   /* need to check because sqlsrv's currently does not correctly */
   status = sqlsrv_sqlda_map_data(x,k,&typ,&len,&scl,
				  (unsigned char **)&data,&nullp,0);
 else
   status = SQLSRV_INVCOLNUM;
 if (status == SQL_SUCCESS)
   {if (nullp)
      result = NIL;
    else
      switch(typ)
	{case SQLSRV_ASCII_STRING:
	 case SQLSRV_GENERALIZED_DATE:
	   result = strcons(len,data);
	   break;
	 case SQLSRV_GENERALIZED_NUMBER:
	   data[len] = 0;
	   d = atof(data);
	   if (scl != 0)
	     d = d * pow(10.0,- (double) scl);
	   result = flocons(d);
	   break;
	 case SQLSRV_VARCHAR:
	   varlen = *((unsigned short *) data);
	   /* this varlen check is just paranoia */
	   if (varlen > len) varlen = len;
	   result = strcons(varlen,&data[2]);
	   break;
	 case SQLSRV_VARBYTE:
	 case SQLSRV_LIST_VARBYTE:
	 default:
	   sqlsrv_sqlda_unmap_data(x,k);
	   err("SQLSRV data type not handled",NIL);}
    sqlsrv_sqlda_unmap_data(x,k);
    no_interrupt(iflag);
    return(result);}
 sqlrtl_status_error(status);}

long get_datum_index(LISP n,LISP alist)
{LISP tmp;
 if NNULLP(numberp(n))
   tmp = n;
 else if NNULLP(tmp = assq(n,alist))
   tmp = cadr(tmp);
 else
   err("not a valid datum index",n);
 return(get_c_long(tmp));}

LISP sqlrtl_get_param(LISP x,LISP n)
{struct statement *c;
 c = get_statement(x);
 return(sqlrtl_get_datum(c->params,get_datum_index(n,c->param_alist)));}

LISP sqlrtl_get_column(LISP x,LISP n)
{struct statement *c;
 c = get_statement(x);
 return(sqlrtl_get_datum(c->selects,get_datum_index(n,c->select_alist)));}

void sqlrtl_put_datum(SQLDA_ID x,long k,LISP value)
{short typ,scl,nullp;
 char *data,*string,num[100],pad = ' ';
 long status,iflag,slen;
 double d;
 unsigned short len;
 iflag = no_interrupt(1);
 if (k < sqlsrv_sqlda_sqld(x))
   /* need to check because sqlsrv's currently does not correctly */
   status = sqlsrv_sqlda_map_data(x,k,&typ,&len,&scl,
				  (unsigned char **)&data,&nullp,0);
 else
   status = SQLSRV_INVCOLNUM;
 if (status != SQL_SUCCESS)
   sqlrtl_status_error(status);
 switch(TYPE(value))
   {case tc_flonum:
      d = FLONM(value);
      if (scl != 0)
	d = d * pow(10.0,(double) scl);
      string = num;
      sprintf(string,"%g",d);
      break;
    case tc_symbol:
    case tc_string:
      string = get_c_string(value);
      break;
    default:
      sqlsrv_sqlda_unmap_data(x,k);
      err("lisp data type not handled",value);}
 slen = strlen(string);
 switch(typ)
   {case SQLSRV_GENERALIZED_DATE:
      pad = '0';
    case SQLSRV_ASCII_STRING:
    case SQLSRV_GENERALIZED_NUMBER:
      /* note: not signalling error on truncation */
      if (slen > len)
	memcpy(data,string,len);
      else
	{memcpy(data,string,slen);
	 if (len > slen)
	   memset(&data[slen],pad,len-slen);}
      break;
    case SQLSRV_VARCHAR:
      if (slen > len)
	/* note: not signalling error on truncation */
	{memcpy(&data[2],string,len);
	 *((unsigned short *) data) = len;}
      else
	{memcpy(&data[2],string,slen);
	 *((unsigned short *) data) = slen;}
      break;
    case SQLSRV_VARBYTE:
    case SQLSRV_LIST_VARBYTE:
    default:
	 sqlsrv_sqlda_unmap_data(x,k);
	 err("SQLSRV data type not handled",NIL);}
 sqlsrv_sqlda_unmap_data(x,k);
 no_interrupt(iflag);}

LISP sqlrtl_set_param(LISP x,LISP n,LISP value)
{struct statement *c;
 c = get_statement(x);
 sqlrtl_put_datum(c->params,
		  get_datum_index(n,c->param_alist),
		  value);
 return(NIL);}

LISP sqlrtl_release_generic(LISP x)
{if ((TYPE(x) != tc_extra) ||
     ((x->storage_as.string.dim != extra_tc_statement) &&
      (x->storage_as.string.dim != extra_tc_association)))
   err("not a statement or association",x);
 if (x->storage_as.string.dim == extra_tc_statement)
   return(sqlrtl_release_statement(x));
 else
   return(sqlrtl_release(x));}

LISP sqlrtl_error_buffer(LISP assoc,LISP resetp)
{struct association *a;
 long iflag,len;
 char *end;
 LISP s;
 a = get_association(assoc,1);
 if NNULLP(resetp)
   {memset(a->error_buffer,0,a->error_buffer_size);
    return(NIL);}
 iflag = no_interrupt(1);
 if (end = memchr(a->error_buffer,0,a->error_buffer_size))
   len = end - a->error_buffer;
 else
   len = a->error_buffer_size;
 s = strcons(len,a->error_buffer);
 no_interrupt(iflag);
 return(s);}


LISP sqlrtl_association_statements(LISP assoc)
{struct association *a;
 a = get_association(assoc,0);
 return(a->statements);}

void extra_gc_scan(LISP ptr)
{struct association *a;
 struct statement *s;
 switch(ptr->storage_as.string.dim)
   {case extra_tc_association:
      a = (struct association *) ptr->storage_as.string.data;
      a->args = gc_relocate(a->args);
      a->statements = gc_relocate(a->statements);
      break;
    case extra_tc_statement:
      s = (struct statement *) ptr->storage_as.string.data;
      s->association = gc_relocate(s->association);
      s->param_alist = gc_relocate(s->param_alist);
      s->select_alist = gc_relocate(s->select_alist);
      s->cursor = gc_relocate(s->cursor);
      break;
    default:
      errswitch();}}

LISP extra_gc_mark(LISP ptr)
{struct association *a;
 struct statement *s;
 switch(ptr->storage_as.string.dim)
   {case extra_tc_association:
      a = (struct association *) ptr->storage_as.string.data;
      gc_mark(a->args);
      gc_mark(a->statements);
      break;
    case extra_tc_statement:
      s = (struct statement *) ptr->storage_as.string.data;
      gc_mark(s->association);
      gc_mark(s->param_alist);
      gc_mark(s->select_alist);
      gc_mark(s->cursor);
      break;
    default:
      errswitch();}
 return(NIL);}

void extra_gc_free(LISP ptr)
     /* release storage allocated. As an extra feature we could
	release associations and statements that were not
	otherwise released. */
{free(ptr->storage_as.string.data);}

void extra_prin1(LISP ptr,struct gen_printio *f)
{struct association *a;
 struct statement *s;
 char buff[512];
 switch(ptr->storage_as.string.dim)
   {case extra_tc_association:
      a = (struct association *) ptr->storage_as.string.data;
      sprintf(buff,"#{SQL ASSOCIATION %p}",a);
      gput_st(f,buff);
      break;
    case extra_tc_statement:
      s = (struct statement *) ptr->storage_as.string.data;
      sprintf(buff,"#{SQL STATEMENT %p}",s);
      gput_st(f,buff);
      break;
    default:
      errswitch();}}

void init_sql_rdb(void)
{long j;
 tc_extra = allocate_user_tc();
 set_gc_hooks(tc_extra,
	      NULL,
	      extra_gc_mark,
	      extra_gc_scan,
	      extra_gc_free,
	      &j);
 set_print_hooks(tc_extra,extra_prin1);
 gc_protect(&associations);
 gc_protect_sym(&sym_ascii_string,"ascii_string");
 gc_protect_sym(&sym_generalized_number,"generalized_number");
 gc_protect_sym(&sym_generalized_date,"generalized_date");
 gc_protect_sym(&sym_varchar,"varchar");
 gc_protect_sym(&sym_varbyte,"varbyte");
 gc_protect_sym(&sym_list_varbyte,"list_varbyte");
 gc_protect_sym(&sym_table,"table");
 gc_protect_sym(&sym_list,"list");
 gc_protect_sym(&sym_update,"update");
 gc_protect_sym(&sym_read_only,"read-only");
 gc_protect_sym(&sym_insert_only,"insert-only");
 gc_protect_sym(&sym_begin,"begin");
 gc_protect_sym(&sym_end,"end");
 gc_protect_sym(&sym_abort,"abort");
 init_subr_1("rdb-describe-association",sqlrtl_describe_association);
 init_subr_1("rdb-describe-statement",sqlrtl_describe_statement);
 init_lsubr("rdb-sql-associate",sqlrtl_associate);
 init_subr_1("rdb-sql-release",sqlrtl_release_generic);
 init_subr_1("rdb-sql-release-statement",sqlrtl_release_statement);
 init_subr_1("rdb-sql-release-association",sqlrtl_release);
 init_subr_0("rdb-sql-associations",sqlrtl_associations);
 init_subr_1("rdb-sql-association-statements",sqlrtl_association_statements);
 init_subr_2("rdb-sql-execute-immediate",sqlrtl_execute_immediate);
 init_subr_2("rdb-sql-prepare",sqlrtl_prepare);
 init_subr_1("rdb-sql-statement-params",sqlrtl_statement_params);
 init_subr_1("rdb-sql-statement-selects",sqlrtl_statement_selects);
 init_subr_1("rdb-sql-statement-association",sqlrtl_statement_association);
 init_subr_3("rdb-sql-declare-cursor",sqlrtl_declare_cursor);
 init_subr_1("rdb-sql-open-cursor",sqlrtl_open_cursor);
 init_subr_1("rdb-sql-close-cursor",sqlrtl_close_cursor);
 init_subr_1("rdb-sql-fetch",sqlrtl_fetch);
 init_subr_2("rdb-sql-fetch-many",sqlrtl_fetch_many);
 init_subr_2("rdb-sql-execute",sqlrtl_execute);
 init_subr_2("rdb-sql-get-param",sqlrtl_get_param);
 init_subr_2("rdb-sql-get-column",sqlrtl_get_column);
 init_subr_3("rdb-sql-set-param",sqlrtl_set_param);
 init_subr_1("rdb-sql-num-batch-rows",sqlrtl_sqlca_num_batch_rows);
 init_subr_1("rdb-sql-count",sqlrtl_sqlca_count);
 init_subr_2("rdb-sql-error-buffer",sqlrtl_error_buffer);
 if (siod_verbose_check(2))
   printf("Enhancements (C) Copyright 1994 Mitech Corporation.\n");
 init_sql_rdb_version();}
