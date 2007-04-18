/* wcsiod.c
   windows console siod main program
   that obtains default argument settings from
   the Registry and uses a dynamic bootstrap technique,
   so that the libsiod.dll does not have to be
   in the normal library search path.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "siod.h"

static char siod_lib_arg[128] = "-l";

static char *siod_argv[7] = {
  "siod",
  "-h100000:10",
  "-g0",
  "-o1000",
  "-s200000",
  "-n2048",
  siod_lib_arg};

static char *library_filename = "libsiod.dll";
static char *registry_key = "SOFTWARE\\George Carrette\\SIOD";

int main(int argc,char **argv,char **env)
{int j,nargc = argc,debugflag = 0;
 char **nargv = argv,*f,*e; 
 HINSTANCE handle; DWORD vtype,vsize;
 int (__stdcall *pmain)(int,char **, char **);
 void (__stdcall *pshuffle_args)(int *,char ***);
 void (__stdcall *pcla)(int,char **,int);
 long status;
 HKEY siodkey;
 for(j=1;j<argc;++j)
  if ((strncmp(argv[j],"-v",2) == 0) &&
      (atol(&argv[j][2]) > 4))
   debugflag = 1;
 if (debugflag) printf("Registry key %s\n",registry_key);
 status = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                       registry_key,
                       0,
                       KEY_QUERY_VALUE,
                       &siodkey);
 if (status == ERROR_SUCCESS)
  {if (debugflag) printf("opened, status %d\n",status);
   vsize = sizeof(siod_lib_arg) - 2;
   status = RegQueryValueEx(siodkey,
                            "SIOD_LIB",
                            NULL,
                            &vtype,
                            &siod_lib_arg[2],
                            &vsize);
   if (status == ERROR_SUCCESS)
    {if (vtype == REG_SZ)
      {if (debugflag) printf("got SIOD_LIB value %s\n",
                             &siod_lib_arg[2]);}
     else
      {if (debugflag) printf("SIOD_LIB value wrong type %d\n",
                             vtype);
       siod_lib_arg[2] = 0;}}
   else if (debugflag)
    printf("failed to get SIOD_LIB, status %d\n",status);
   status = RegCloseKey(siodkey);
   if (debugflag) printf("key closed, status %d\n",status);
}
else if (debugflag)
 printf("key not opened, error %d\n",status);
           
/* let the old style environment variable over-ride the
   registry setting. This is historical and may be useful
   for debugging. */  

 if (((e = getenv("SIOD_LIB")) && *e) ||
     ((e = &siod_lib_arg[2]) && *e))
  {f = (char *) malloc(strlen(e)+strlen(library_filename)+1);
   sprintf(f,"%s%s",e,library_filename);}
  else
   f = library_filename;
 
 if (!(handle = LoadLibraryEx(f,NULL,LOAD_WITH_ALTERED_SEARCH_PATH)))
  {fprintf(stderr,"error code %d, cannot load library %s\n",
           GetLastError(),f);
   return(EXIT_FAILURE);}
 if (!((pmain = (LPVOID)GetProcAddress(handle,"siod_main")) &&
       (pshuffle_args = (LPVOID) GetProcAddress(handle,"siod_shuffle_args")) &&
       (pcla = (LPVOID) GetProcAddress(handle,"process_cla"))))
  {fprintf(stderr,"error code %d, cannot find siod calls in library %s\n",
                  GetLastError(),f);
   return(EXIT_FAILURE);}
 if (nargc > 0)
  (*pshuffle_args)(&nargc,&nargv);
 (*pcla)((sizeof(siod_argv)/sizeof(char *)) -
         ((siod_lib_arg[2]) ? 0 : 1),
         siod_argv,1);
 return((*pmain)(nargc,nargv,env));}









