/* provide runtime resolution of ct library symbols.
 */


#include <stdio.h>
#include <string.h>
#include <ctpublic.h>
#include "siod.h"

struct a_cs_retcode
{CS_RETCODE n;
 char *name;};

struct a_cs_int
{CS_INT n;
 char *name;};

static struct a_cs_retcode retcodes[] = {
  {CS_SUCCEED, "CS_SUCCEED"},
  {CS_FAIL, "CS_FAIL"},
  {CS_MEM_ERROR, "CS_MEM_ERROR"},
  {CS_PENDING, "CS_PENDING"},
  {CS_QUIET, "CS_QUIET"},
  {CS_BUSY, "CS_BUSY"},
  {CS_INTERRUPT, "CS_INTERRUPT"},
  {CS_BLK_HAS_TEXT, "CS_BLK_HAS_TEXT"},
  {CS_CONTINUE, "CS_CONTINUE"},
  {CS_FATAL, "CS_FATAL"},
  {CS_CANCELED, "CS_CANCELED"},
  {CS_ROW_FAIL, "CS_ROW_FAIL"},
  {CS_END_DATA, "CS_END_DATA"},
  {CS_END_RESULTS, "CS_END_RESULTS"},
  {CS_END_ITEM, "CS_END_ITEM"},
  {CS_NOMSG, "CS_NOMSG"},
  {CS_TIMED_OUT, "CS_TIMED_OUT"},
  {CS_PASSTHRU_EOM, "CS_PASSTHRU_EOM"},
  {CS_PASSTHRU_MORE, "CS_PASSTHRU_MORE"},
  {CS_TRYING, "CS_TRYING"},
  {CS_EBADPARAM, "CS_EBADPARAM"},
  {CS_EBADLEN, "CS_EBADLEN"},
  {CS_ENOCNVRT, "CS_ENOCNVRT"},
  {CS_EOVERFLOW, "CS_EOVERFLOW"},
  {CS_EUNDERFLOW, "CS_EUNDERFLOW"},
  {CS_EPRECISION, "CS_EPRECISION"},
  {CS_ESCALE, "CS_ESCALE"},
  {CS_ESYNTAX, "CS_ESYNTAX"},
  {CS_EFORMAT, "CS_EFORMAT"},
  {CS_EDOMAIN, "CS_EDOMAIN"},
  {CS_EDIVZERO, "CS_EDIVZERO"},
  {CS_ERESOURCE, "CS_ERESOURCE"},
  {CS_ENULLNOIND, "CS_ENULLNOIND"},
  {CS_ETRUNCNOIND, "CS_ETRUNCNOIND"},
  {CS_ENOBIND, "CS_ENOBIND"},
  {CS_TRUNCATED, "CS_TRUNCATED"},
  {CS_ESTYLE, "CS_ESTYLE"},
  {CS_EBADXLT, "CS_EBADXLT"},
  {CS_ENOXLT, "CS_ENOXLT"}};

static struct a_cs_int cmds[] = {
  {CS_LANG_CMD, "CS_LANG_CMD"},
  {CS_RPC_CMD, "CS_RPC_CMD"},
  {CS_MSG_CMD, "CS_MSG_CMD"},
  {CS_SEND_DATA_CMD, "CS_SEND_DATA_CMD"},
  {CS_PACKAGE_CMD, "CS_PACKAGE_CMD"},
  {CS_SEND_BULK_CMD, "CS_SEND_BULK_CMD"}};

static struct a_cs_int types[] = {
  {CS_ILLEGAL_TYPE, "CS_ILLEGAL_TYPE"},
  {CS_CHAR_TYPE, "CS_CHAR_TYPE"},
  {CS_BINARY_TYPE, "CS_BINARY_TYPE"},
  {CS_LONGCHAR_TYPE, "CS_LONGCHAR_TYPE"},
  {CS_LONGBINARY_TYPE, "CS_LONGBINARY_TYPE"},
  {CS_TEXT_TYPE, "CS_TEXT_TYPE"},
  {CS_IMAGE_TYPE, "CS_IMAGE_TYPE"},
  {CS_TINYINT_TYPE, "CS_TINYINT_TYPE"},
  {CS_SMALLINT_TYPE, "CS_SMALLINT_TYPE"},
  {CS_INT_TYPE, "CS_INT_TYPE"},
  {CS_REAL_TYPE, "CS_REAL_TYPE"},
  {CS_FLOAT_TYPE, "CS_FLOAT_TYPE"},
  {CS_BIT_TYPE, "CS_BIT_TYPE"},
  {CS_DATETIME_TYPE, "CS_DATETIME_TYPE"},
  {CS_DATETIME4_TYPE, "CS_DATETIME4_TYPE"},
  {CS_MONEY_TYPE, "CS_MONEY_TYPE"},
  {CS_MONEY4_TYPE, "CS_MONEY4_TYPE"},
  {CS_NUMERIC_TYPE, "CS_NUMERIC_TYPE"},
  {CS_DECIMAL_TYPE, "CS_DECIMAL_TYPE"},
  {CS_VARCHAR_TYPE, "CS_VARCHAR_TYPE"},
  {CS_VARBINARY_TYPE, "CS_VARBINARY_TYPE"},
  {CS_LONG_TYPE, "CS_LONG_TYPE"},
  {CS_SENSITIVITY_TYPE, "CS_SENSITIVITY_TYPE"},
  {CS_BOUNDARY_TYPE, "CS_BOUNDARY_TYPE"}};

char *sybase_retstr(CS_RETCODE n)
{long j,m;
 m = sizeof(retcodes) / sizeof(struct a_cs_retcode);
 for(j=0;j<m;++j)
   if (n == retcodes[j].n)
     return(retcodes[j].name);
 return(NULL);}

char *sybase_typestr(CS_INT n)
{long j,m;
 m = sizeof(types) / sizeof(struct a_cs_int);
 for(j=0;j<m;++j)
   if (n == types[j].n)
     return(types[j].name);
 return(NULL);}


void init_sql_sybasec(void)
{long j,n;
 n = sizeof(retcodes) / sizeof(struct a_cs_retcode);
 for(j=0;j<n;++j)
   setvar(cintern(retcodes[j].name),flocons(retcodes[j].n),NIL);
 n = sizeof(cmds) / sizeof(struct a_cs_int);
 for(j=0;j<n;++j)
   setvar(cintern(cmds[j].name),flocons(cmds[j].n),NIL);}
