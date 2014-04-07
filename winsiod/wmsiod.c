/* wmsiod.c

Provide a WinMain function so that SIOD procedures can be
executed from a WIN32 application.
This is one way to avoid the "command prompt" window that usually
pops up with a WIN32 console application.

We do this by creating a compatible argv/argc and calling
the "main" subroutine from wcsiod.c

*/  
  
#include <windows.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main (int argc, char **argv, char **env);
 static int
call_main (void) 
{
  LPWSTR *argvw, l;
  int argc, j, n, res;
  char **argv, **oargv, **env;
  l = GetCommandLineW ();
  argvw = CommandLineToArgvW (l, &argc);
  argv = (char **) malloc (argc * sizeof (char *));
  oargv = (char **) malloc (argc * sizeof (char *));
  for (j = 0; j < argc; ++j)
    
    {
      n = wcslen (argvw[j]);
      argv[j] = (char *) malloc (n + 1);
      memset (argv[j], 0, n + 1);
      wcstombs (argv[j], argvw[j], n + 1);
      oargv[j] = argv[j];
    } 
    /* most siod programs use getenv rather than look at *env*
       so it shouldn't hurt to just fake out the envp with an empty list.
       TODO: There may be a documented way to pick this info up.
     */ 
    env = (char **) malloc (1 * sizeof (char *));
  env[0] = NULL;
  res = main (argc, argv, env);
  for (j = 0; j < argc; ++j)
    free (oargv[j]);
  free (argv);
  free (oargv);
  for (j = 0; env[j]; ++j)
    free (env[j]);
  free (env);
  LocalFree (argvw);
  return (res);
}

  
/* TODO: Something useful with the other arguments,
   especially so that Brian Beckman's stuff.
   See http://www.angelfire.com/wa/brianbec/siodffi.html
*/ 
  int APIENTRY
WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
	 int nCmdShow) 
{
  return (call_main ());
}

 
