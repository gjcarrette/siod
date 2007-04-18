/* win32 releated api utilities.
   Trying to get by with a minimum amount of stuff here.
   Registry facilities are important for i
   nstallation.
   Getting access to functionality via COM/DCOM covers a huge amount
   of ground.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "siod.h"

/* Monikers of the form file:, url: and clsid: are common */

#if 0

LISP l_oletest1(LISP name)
{IBindCtx *bc = NULL;
 DWORD status;
 HRESULT hr;
 ULONG FAR eaten;
 LPMONIKER FAR imoniker;
 hr = CreateBindCtx(0,&bc);
 if (!SUCCEEDED(hr))
   return(err("CreateBindCtx",llast_win32_errmsg(hr)));
 if (!bc)
   return(err("CreateBindCtx NULL",NIL));

 hr = MkParseDisplayName(bc,displayname,&eaten,&imoniker);
 if (!SUCCEEDED(hr))
   return(err("MkParseDisplayName",llast_win32_errmsg(hr)));
 
 
 /* status = MkParseDisplayName(); */
 return(NIL);}

#endif


static HKEY rkeylookup(char *name)
{if (strcmp(name,"HKEY_LOCAL_MACHINE") == 0)
  return(HKEY_LOCAL_MACHINE);
 else if (strcmp(name,"HKEY_CLASSES_ROOT")==0)
  return(HKEY_CLASSES_ROOT);
 else if (strcmp(name,"HKEY_CURRENT_CONFIG")==0)
  return(HKEY_CURRENT_CONFIG);
 else if (strcmp(name,"HKEY_CURRENT_USER")==0)
  return(HKEY_CURRENT_USER);
 else if (strcmp(name,"HKEY_USERS")==0)
   return(HKEY_USERS);
 else return(INVALID_HANDLE_VALUE);}

LISP lregistry_ref(LISP rootname,LISP keyname,LISP valuename)
/* Convenient, not efficient, procedure for fetching registry values */
{long iflag,status,status2;
 DWORD vtype,vsize;
 HKEY rkey,key;
 LISP temp = NIL;
 char *crootname,*ckeyname,*cvaluename,littlebuffer[256],*bigbuffer,*buff;
 crootname = get_c_string(rootname);
 ckeyname = get_c_string(keyname);
 cvaluename = get_c_string(valuename);
 iflag = no_interrupt(1);
 if ((rkey = rkeylookup(crootname)) == INVALID_HANDLE_VALUE)
   return(err("root handle name not implemented",rootname));
 status = RegOpenKeyEx(rkey,
                       ckeyname,
                       0,
                       KEY_QUERY_VALUE,
                       &key);
 if (status != ERROR_SUCCESS)
  return(err("RegOpenKeyEx",llast_win32_errmsg(status)));
 vsize = sizeof(littlebuffer);
 buff = littlebuffer;
 status = RegQueryValueEx(key,
                          cvaluename,
                          NULL,
                          &vtype,
                          buff,
                          &vsize);
 if (status == ERROR_MORE_DATA)
 {temp = strcons(vsize,NULL);
  bigbuffer = get_c_string(temp);
  buff = bigbuffer;
  status = RegQueryValueEx(key,
                          cvaluename,
                          NULL,
                          &vtype,
                          buff,
                          &vsize);}
  status2 = RegCloseKey(key);
  if (status2 != ERROR_SUCCESS)
   return(err("RegCloseKey",llast_win32_errmsg(status2)));
  no_interrupt(iflag);
  if (status != ERROR_SUCCESS)
   return(err("RegQueryValueEx",llast_win32_errmsg(status)));
  switch(vtype)
  {case REG_SZ:
   case REG_EXPAND_SZ:
    if (temp) return(temp);
	return(strcons(vsize-1,buff));
   case REG_BINARY:
   case REG_NONE:
    if (temp) return(temp);
	return(strcons(vsize,buff));
   case REG_DWORD:
	return(flocons(*((DWORD *)buff)));
   default: return(err("data type not handled",flocons(vtype)));}

}

static LISP lregistry_set(LISP rootname,LISP keyname,LISP valuename,LISP val)
{long iflag,status,status2,len;
 DWORD vsize,vdisp;
 HKEY rkey,key;
 LISP temp = NIL;
 char *crootname,*ckeyname,*cvaluename,*cval;
 crootname = get_c_string(rootname);
 ckeyname = get_c_string(keyname);
 cvaluename = get_c_string(valuename);
 cval = get_c_string_dim(val,&len);
 iflag = no_interrupt(1);
 if ((rkey = rkeylookup(crootname)) == INVALID_HANDLE_VALUE)
   return(err("root handle name not implemented",rootname));
 status = RegCreateKeyEx(rkey,
			 ckeyname,0,
			 "classname",
			 REG_OPTION_NON_VOLATILE,
			 KEY_ALL_ACCESS,NULL,&key,&vdisp);
 if (status != ERROR_SUCCESS)
   return(err("RegCreateKeyEx",llast_win32_errmsg(status)));
 vsize = len + 1;
 status = RegSetValueEx(key,cvaluename,0,
			REG_SZ,
			cval,
			vsize);
 status2 = RegCloseKey(key);
 if (status2 != ERROR_SUCCESS)
  return(err("RegCloseKey",llast_win32_errmsg(status2)));
 if (status != ERROR_SUCCESS)
  return(err("RegSetValueEx",llast_win32_errmsg(status)));
 
 no_interrupt(iflag);
 return(val);}

static LISP win32_errmsg(LISP code)
{DWORD status;
 status = get_c_long(code);
 return(llast_win32_errmsg(status));}

     
__declspec(dllexport) void init_slibw32(void)
{init_subr_3("registry-ref",lregistry_ref);
 init_subr_4("registry-set",lregistry_set);
 init_subr_1("win32-errmsg",win32_errmsg);
#if 0
 init_subr_1("ole-test1",l_oletest1);
#endif
}


 
 
