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

/* Interface from SIOD to Oracle Call Interface.
   20-JAN-94 George Carrette. GJC@MITECH.COM

   This was developed some time ago using Oracle 6.0

Building: Compile siod.c with #define INIT_EXTRA init_sql_oracle. 
          Linking is more complex as Oracle supplies specialized
          script files. Under VMS it would look something like:

$ @ORA_RDBMS:LNOCIC SIOD.EXE SIOD.OBJ,SLIB.OBJ,SLIBA.OBJ,-
TRACE.OBJ,SQL_ORACLE.OBJ "S"

TO DO: (1) Deal with parameters at LISP type level. The hairy
           procedure prepare_statement can handle it already.
       (2) allow more than one oracle login by encapsulating lda and hda
           and passing them around.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>

#include "siod.h"

static void init_sql_oracle_version(void)
{setvar(cintern("*sql_oracle-version*"),
	cintern("$Id: sql_oracle.c,v 1.2 1998/10/05 18:10:47 gjcarret Exp $"),
	NIL);}


static long tc_extra = 0;
#define extra_tc_association 1
#define extra_tc_statement 2

/* Oracle does not seem to provide any useful .h files for the
   lda and cursor structures, nor for the data types or symbolic
   error codes, nor procedure prototypes. All of which makes for a lot
   of extra work and non-uniformity in the examples provided in the
   Oracle documentation.
*/

struct lda_def
{short v2_rc;
 unsigned char fill1[10];
 unsigned short rc;
 unsigned char fill2[19];
 unsigned int ose;
 unsigned char chk;
 unsigned char sysparm[26];};

struct cda_def
{short v2_rc;
 short ft;
 unsigned long rpc;
 short peo;
 unsigned char fc;
 unsigned char fill1;
 unsigned short rc;
 unsigned char wrn;
 unsigned char flg;
 unsigned int cn;
 unsigned char rid[13];
 unsigned int ose;
 unsigned char chk;
 unsigned char sysparm[26];};

struct hda_def
{char fill[256];};

#define ORACLE_ETYPE_CHAR       1
#define ORACLE_ITYPE_NUMBER     2
#define ORACLE_ETYPE_INTEGER    3
#define ORACLE_ETYPE_FLOAT      4
#define ORACLE_ETYPE_STRING     5
#define ORACLE_ETYPE_DECIMAL    7
#define ORACLE_ETYPE_LONG       8
#define ORACLE_ETYPE_VARCHAR    9
#define ORACLE_ETYPE_ROWID     11
#define ORACLE_ETYPE_DATE      12
#define ORACLE_ETYPE_VARRAW    15
#define ORACLE_ETYPE_RAW       23
#define ORACLE_ETYPE_LONGRAW   24
#define ORACLE_ETYPE_UINT      68
#define ORACLE_ETYPE_DISPLAY   91

#define ORACLE_MAX_STRING 255
#define ORA_MAX_SELECTS 200 /* this may be something I made up */

struct oracle_date
{unsigned char century;
 unsigned char year;
 unsigned char month;
 unsigned char day;
 unsigned char hour;
 unsigned char minute;
 unsigned char second;};

#define ORA_NO_DATA_FOUND      1403
#define ORA_VAR_NOT_IN_SELECT  1007

/* This code should be restructured to allocate lda and hda
   in an association data structure returned by l_orlon */

static struct lda_def lda;
static struct hda_def hda;

static long orlon_ok = 0;

static char *errmsg_str0 = NULL;
static char *errmsg_str1 = NULL;

static long ncursors = 0;

struct param
{short etype; 
 short buflen;
 char *buf;};

struct select
{char *colnam;
 short dbtype;
 short dbsize;
 short dsize;
 short etype; 
 short buflen;
 char *buf;
 short fetchlenstat;
 short fetchlen;
 short fetchcode;};

struct cstatement
{struct cda_def *cursor;
 long nparams;
 struct param *params;
 long nselects;
 struct select *selects;};

static char *errmsg(long code)
{char *ptr;
 if (errmsg_str0 == NULL)
   {errmsg_str0 = (char *) malloc(ORACLE_MAX_STRING+1);
    memset(errmsg_str0,0,ORACLE_MAX_STRING+1);}
 oerhms(&lda,code,errmsg_str0,ORACLE_MAX_STRING);
 if (ptr = strchr(errmsg_str0,'\n')) *ptr = 0;
 return(errmsg_str0);}

char *cda_errmsg(struct cda_def *cur)
{return(errmsg(cur->rc));}

char *lda_errmsg()
{return(errmsg(lda.rc));}

LISP ferr(LISP obj,char *format, ...)
{va_list restargs;
 if (errmsg_str1 == NULL)
   {errmsg_str1 = (char *) malloc((ORACLE_MAX_STRING*3)+1);
    memset(errmsg_str1,0,(ORACLE_MAX_STRING*3)+1);}
 va_start(restargs,format);
 vsprintf(errmsg_str1,format,restargs);
 va_end(restargs);
 err(errmsg_str1,obj);}

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

struct cda_def *allocate_cursor(void)
{struct cda_def *cur;
 unsigned short rc;
 cur = (struct cda_def *) malloc(sizeof(struct cda_def));
 if (oopen(cur,&lda,NULL,-1,-1,NULL,-1))
   {rc = cur->rc;
    free(cur);
    ferr(NIL,"%d cursors so far. oopen:\n%s",
	 ncursors,errmsg(rc));}
 ++ncursors;
 return(cur);}

void free_cursor(struct cda_def *cur)
{long result;
 unsigned short rc;
 result = oclose(cur);
 rc = cur->rc;
 free(cur);
 if (result) ferr(NIL,"oclose: %s",errmsg(rc));}

LISP l_orlon(LISP username,LISP password)
{long retval,flag;
 char *c_username,*c_password;
 c_username = get_c_string(username);
 c_password = NNULLP(password) ? get_c_string(password) : NULL;
 if (orlon_ok)
   /* calling orlon twice will corrupt your process badly. */
   err("already completed orlon",NIL);
 else
   {flag = no_interrupt(1);
    retval = orlon(&lda,&hda,
		   c_username,strlen(c_username),
		   c_password,(c_password) ? strlen(c_password) : -1,
		   0);
    no_interrupt(flag);
    if (lda.rc)
      ferr(NIL,"orlon: %s",lda_errmsg());
    else
      orlon_ok = 1;}
 return(NIL);}

LISP l_ologof(void)
{long flag;
 flag = no_interrupt(1);
 if (orlon_ok)
   {ologof(&lda);
    if (lda.rc)
      ferr(NIL,"orlon: %s",lda_errmsg());
    else
      orlon_ok = 0;}
 no_interrupt(flag);
 return(NIL);}

void freeloc(void ** x)
{if (*x)
   {free(*x);
    *x = NULL;}}

void release_statement(struct cstatement *c)
{long j;
 if (c->params)
   {for(j = 1;j <= c->nparams;++j)
      freeloc(&c->params[j-1].buf);
    freeloc(&c->params);}
 if (c->selects)
   {for(j = 1; j <= c->nselects; ++j)
      {freeloc(&c->selects[j-1].colnam);
       freeloc(&c->selects[j-1].buf);}
    freeloc(&c->selects);}
 if (c->cursor)
   free_cursor(c->cursor);
 c->cursor = NULL;}

void prepare_statement(char *sql_str,struct cstatement *c)
     /* assumptions:
	nparams gives range of params from :1 to :nparams
	the user may pre-initialize params with the etypes requested. 
	nselects is correct or -1                                     */
{long j,sflag = 0;
 short colnamlen;
 char colnam[ORACLE_MAX_STRING+1];
 char *err;
 c->cursor = allocate_cursor();
 if (osql3(c->cursor,sql_str,-1))
   {err = cda_errmsg(c->cursor);
    free_cursor(c->cursor);
    ferr(NIL,"osql3: %s",err);}
 if (c->nparams)
   {if (!c->params)
      {c->params = (struct param *) malloc(sizeof(struct param) * c->nparams);
       for(j = 1;j <= c->nparams;++j)
	 {c->params[j-1].etype = 0;
	  c->params[j-1].buf = NULL;}}
    else
      for(j = 1;j <= c->nparams;++j)
	c->params[j-1].buf = NULL;
    for(j = 1;j <= c->nparams;++j)
      {switch(c->params[j-1].etype)
	 {case ORACLE_ETYPE_DATE:
	    c->params[j-1].buflen = sizeof(struct oracle_date);
	    c->params[j-1].buf = (char *) malloc(c->params[j-1].buflen);
	    break;
	  default:
	    c->params[j-1].etype = ORACLE_ETYPE_STRING;
	    c->params[j-1].buflen = ORACLE_MAX_STRING;
	    c->params[j-1].buf = (char *) malloc(c->params[j-1].buflen+1);}
       if (obndrn(c->cursor,j,
		  c->params[j-1].buf,
		  (c->params[j-1].etype == ORACLE_ETYPE_STRING)
		  ? -1 : c->params[j-1].buflen,
		  c->params[j-1].etype,
		  -1,NULL,NULL,-1,-1))
	 {err = cda_errmsg(c->cursor);
	  release_statement(c);
	  ferr(NIL,"obndrn %d: %s",j,err);}}}
 else
   c->params = NULL;
 if (c->nselects)
   {if (c->nselects < 0)
      {sflag = 1;
       c->nselects = ORA_MAX_SELECTS;}
    c->selects = (struct select *) malloc(sizeof(struct select) * c->nselects);
    memset(c->selects,0,sizeof(struct select) * c->nselects);
    for(j = 1; j <= c->nselects; ++j)
      {colnamlen = ORACLE_MAX_STRING;
       if (odsc(c->cursor,j,
		&c->selects[j-1].dbsize,
		NULL,NULL,
		&c->selects[j-1].dbtype,
		colnam,&colnamlen,
		&c->selects[j-1].dsize))
	 {if ((!sflag) ||
	      (c->cursor->rc != ORA_VAR_NOT_IN_SELECT))
	    {err = cda_errmsg(c->cursor);
	     release_statement(c);
	     ferr(NIL,"odsc %d: %s",j,err);}
	  c->nselects = j-1;}
       else
	 {colnam[colnamlen] = 0;
	  c->selects[j-1].colnam = (char *) malloc(colnamlen+1);
	  strcpy(c->selects[j-1].colnam,colnam);
	  switch(c->selects[j-1].dbtype)
	    {case ORACLE_ETYPE_INTEGER:
	     case ORACLE_ETYPE_FLOAT:
	     case ORACLE_ITYPE_NUMBER:
	       c->selects[j-1].etype = ORACLE_ETYPE_FLOAT;
	       c->selects[j-1].buflen = sizeof(double);
	       c->selects[j-1].buf = (double *) malloc(c->selects[j-1].buflen);
	       *((double *)c->selects[j-1].buf) = 0.0;
	       break;
	     case ORACLE_ETYPE_DATE:
	       /* If we let Oracle convert to string we loose the time info */
	       c->selects[j-1].etype = ORACLE_ETYPE_DATE;
	       c->selects[j-1].buflen = sizeof(struct oracle_date);
	       c->selects[j-1].buf = (char *) malloc(c->selects[j-1].buflen);
	       break;
	     default:
	       c->selects[j-1].etype = ORACLE_ETYPE_STRING;
	       c->selects[j-1].buflen =  ORACLE_MAX_STRING;
	       c->selects[j-1].buf = (char *) malloc(c->selects[j-1].buflen+1);
	       c->selects[j-1].buf[0] = 0;}
	  if (odefin(c->cursor,j,
		     c->selects[j-1].buf,c->selects[j-1].buflen,
		     c->selects[j-1].etype,
		     -1,
		     &c->selects[j-1].fetchlenstat,
		     NULL,-1,-1,
		     &c->selects[j-1].fetchlen,
		     &c->selects[j-1].fetchcode))
	    {err = cda_errmsg(c->cursor);
	     release_statement(c);
	     ferr(NIL,"odefin %d: %s",j,err);}}}
    if (c->nselects == 0)
      {free(c->selects);
       c->selects = NULL;}}
 else
   c->selects = NULL;}

LISP oracle_sql_prepare(LISP str)
{long iflag;
 LISP result;
 struct cstatement *c;
 iflag = no_interrupt(1);
 result = extcons(sizeof(struct cstatement),extra_tc_statement);
 c = (struct cstatement *) result->storage_as.string.data;
 c->nparams = 0;
 c->nselects = -1;
 prepare_statement(get_c_string(str),c);
 no_interrupt(iflag);
 return(result);}

static struct cstatement *get_cstatement(LISP st)
{struct cstatement *c;
 if ((TYPE(st) != tc_extra) ||
     (st->storage_as.string.dim != extra_tc_statement))
   err("not a statement",st);
 c = (struct cstatement *)st->storage_as.string.data;
 if (!c->cursor)
   err("statement has been released",st);
 return(c);}

LISP oracle_sql_release(LISP s)
{long iflag;
 iflag = no_interrupt(1);
 release_statement(get_cstatement(s));
 no_interrupt(iflag);
 return(NIL);}

LISP oracle_execute(LISP s)
{long iflag;
 struct cstatement *c;
 iflag = no_interrupt(1);
 c = get_cstatement(s);
 if (oexec(c->cursor))
   ferr(s,"oexec: %s",cda_errmsg(c->cursor));
 no_interrupt(iflag);
 return(NIL);}

LISP oracle_nselects(LISP s)
{return(flocons((get_cstatement(s))->nselects));}

LISP oracle_select_column_name(LISP s,LISP n)
{long j;
 struct cstatement *c;
 j = get_c_long(n);
 c = get_cstatement(s);
 if ((j<0) || (j >= c->nselects))
   err("column index out of range",n);
 return(rintern(c->selects[j].colnam));}

char *oracle_date_to_string(struct oracle_date *d)
     /* make it look like the string returned by RDB SQL Services */
{static char buff[100];
 sprintf(buff,"%02d%02d%02d%02d%02d%02d%02d00",
	 d->century - 100,d->year - 100,
	 d->month,d->day,
	 d->hour-1,d->minute-1,d->second-1);
 return(buff);}

LISP oracle_select_column_value(LISP s,LISP n)
{long j;
 struct cstatement *c;
 struct select *sel;
 char *str;
 j = get_c_long(n);
 c = get_cstatement(s);
 if ((j<0) || (j >= c->nselects))
   err("column index out of range",n);
 sel = &c->selects[j];
 if (sel->fetchlenstat < 0)
   return(NIL);
 switch(sel->etype)
   {case ORACLE_ETYPE_FLOAT:
      return(flocons(*((double *)sel->buf)));
    case ORACLE_ETYPE_DATE:
      str = oracle_date_to_string(sel->buf);
      return(strcons(strlen(str),str));
    case ORACLE_ETYPE_STRING:
      return(strcons(sel->fetchlen,sel->buf));
    default:
      return(errswitch());}}

LISP oracle_fetch(LISP s)
{long iflag;
 struct cstatement *c;
 c = get_cstatement(s);
 iflag = no_interrupt(1);
 if (ofetch(c->cursor))
   {if (c->cursor->rc == ORA_NO_DATA_FOUND)
      {no_interrupt(iflag);
       return(NIL);}
    ferr(s,"fetch: %s",cda_errmsg(c->cursor));}
 no_interrupt(iflag);
 return(s);}

static void extra_gc_free(LISP ptr)
{struct cstatement *c;
 c = (struct cstatement *) ptr->storage_as.string.data;
 release_statement(c);
 free(c);}

static void extra_prin1(LISP ptr,struct gen_printio *f)
{struct cstatement *c;
 char buff[512];
 switch(ptr->storage_as.string.dim)
   {case extra_tc_statement:
      c = (struct cstatement *) ptr->storage_as.string.data;
      if (c->cursor)
	sprintf(buff,"#{SQL STATEMENT %p cursor %d}",
		c,c->cursor->cn);
      else
	sprintf(buff,"#{SQL STATEMENT %p released}",c);
      gput_st(f,buff);
      break;
    default:
      errswitch();}}

void init_sql_oracle(void)
{long j;
 tc_extra = allocate_user_tc();
 set_gc_hooks(tc_extra,NULL,NULL,NULL,extra_gc_free,&j);
 set_print_hooks(tc_extra,extra_prin1);
 init_subr_2("oracle-login",l_orlon);
 init_subr_0("oracle-logout",l_ologof);
 init_subr_1("oracle-sql-prepare",oracle_sql_prepare);
 init_subr_1("oracle-sql-release",oracle_sql_release);
 init_subr_1("oracle-execute",oracle_execute);
 init_subr_1("oracle-nselects",oracle_nselects);
 init_subr_2("oracle-select-column-name",oracle_select_column_name);
 init_subr_2("oracle-select-column-value",oracle_select_column_value);
 init_subr_1("oracle-fetch",oracle_fetch);
 if (siod_verbose_check(2))
  put_st("Enhancements (C) Copyright 1994 Mitech Corporation.\n");
 init_sql_oracle_version();}

