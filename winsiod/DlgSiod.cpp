#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>

#include <stdlib.h>
#include <crtdbg.h>
#include <tchar.h>
#include <stdio.h>   
#include <string.h>  
#include <ctype.h>

#include "DlgSiodRC.H"
#include "DlgUtil.H"
#include "siod.h"
/*///////////////////////////////////////////////////////////////////////////////
                    ____  _       _     _     _    ___   __
                   | __ )(_) __ _| |   (_)___| |_ / _ \ / _|
                   |  _ \| |/ _` | |   | / __| __| | | | |_
                   | |_) | | (_| | |___| \__ \ |_| |_| |  _|
                   |____/|_|\__, |_____|_|___/\__|\___/|_|
                   _   _ _  |___/__   ___  _   _ _____ ____
                  | | | | \ | |  _ \ / _ \| \ | | ____/ ___|
                  | | | |  \| | | | | | | |  \| |  _| \___ \
                  | |_| | |\  | |_| | |_| | |\  | |___ ___) |
                   \___/|_| \_|____/ \___/|_| \_|_____|____/

1. Thread safety of the global variables g_quitNow, g_hSiodReplThread, and 
   g_NewSiodThreadNeeded is not certain.  In particular, by leaning on the 
   "FreshSiod" button, we can get multiple threads running inside SIOD at 
   once, and we know this is bad. 

2. LibSiod.dll must be unloaded when the SIOD thread terminates.  Otherwise,
   SIOD must be made internally thread safe.  This latter is a much larger
   job. 

3. Some UI for SIOD's argv's (cute but not important)  

///////////////////////////////////////////////////////////////////////////////*/




//-----------------------------------------------------------------------------
//                            H  A  N  D  L  E  S
//-----------------------------------------------------------------------------
HANDLE g_hSiodReplThread ;	DWORD g_dwSiodThreadID ;
HANDLE g_hLoaderThread   ;	DWORD g_dwLoaderThreadId   ;

HANDLE g_charArrivedEvent  = 0  ;

HWND   g_hWndListener         ;
HWND   g_hWndEdit             ;
HWND   g_hDialog              ;

//-----------------------------------------------------------------------------
//           B  U  F  F  E  R  S     A  N  D     C  U  R  S  O  R  S
//-----------------------------------------------------------------------------
const DWORD  g_cbPageSize = 4096 ;

char    g_szTemp[g_cbPageSize] ;
#define GET_LIMIT   g_cbPageSize
#define UNGET_LIMIT g_cbPageSize

char  g_ungetBuffer [UNGET_LIMIT] ;
DWORD g_ungetIndex  = 0           ;

char  g_getBuffer   [ GET_LIMIT ] ;
DWORD g_getTail     = 0xffffffff  ;
DWORD g_getHead     = 0xffffffff  ;

//-----------------------------------------------------------------------------
//      F  U  N  C  T  I  O  N  S     A  N  D     F  U  N  C  P  T  R  S
//-----------------------------------------------------------------------------
DWORD WINAPI ThreadSiodRepl   (LPVOID lpv                       ) ;
void         CreateSiodThread (                                 ) ;
int          SiodMain         (int argc, char **argv, char **env) ;
void         InitEvents       (                                 ) ;
DWORD        LastErrorBox     (                                 ) ;

WNDPROC      g_oldListenerWndPrc = 0 ;

//-----------------------------------------------------------------------------
//       S  Y  N  C  H  R  O  N  I  Z  A  T  I  O  N     F  L  A  G  S
//-----------------------------------------------------------------------------
static int g_quitNow             = 0 ;  
static int g_NewSiodThreadNeeded = 0 ;

//-----------------------------------------------------------------------------
//                            S  T  R  I  N  G  S
//-----------------------------------------------------------------------------
CHAR szOriginalVictimModule[]     = { "..\\..\\..\\bin\\Debug\\simplesuspect.dll" } ;
CHAR szOriginalShortVictim[]      = { "simplesuspect.dll" } ;
CHAR szVictimModule [MAX_PATH+1]  = { "..\\..\\..\\bin\\Debug\\simplesuspect.dll" } ;


/////////////////////////////////////////////////////////////////////////////////
/* * /
                          ____              ___
                         |  _ \ _   _ _ __ |_ _|_ __
                         | |_) | | | | '_ \ | || '_ \
                         |  _ <| |_| | | | || || | | |
                _        |_| \_\\__,_|_| |_|___|_| |_|          _
               / \   _ __  _   _|_   _| |__  _ __ ___  __ _  __| |
              / _ \ | '_ \| | | | | | | '_ \| '__/ _ \/ _` |/ _` |
             / ___ \| | | | |_| | | | | | | | | |  __/ (_| | (_| |
            /_/   \_\_| |_|\__, | |_| |_| |_|_|  \___|\__,_|\__,_|
                           |___/
/* */

void CreateSiodThread ()
{
	//
	// UNDONE: Synchro hole, here.  This variable should be set atomically.
	//

	g_hSiodReplThread = CreateThread
	(
		NULL,				// No Security Attributes, please
		0,					// Default stack size, please
		ThreadSiodRepl,		// Run this function, please
		(LPVOID) 27,        // Function, do this many iterations, please
		0,					// Wake up now 
		&g_dwSiodThreadID	// Must have an ID on Win95
	);

	if (g_hSiodReplThread == NULL)
		LastErrorBox () ;
}


/////////////////////////////////////////////////////////////////////////////////
/* * /
                          ____              ___
                         |  _ \ _   _ _ __ |_ _|_ __
                         | |_) | | | | '_ \ | || '_ \
                         |  _ <| |_| | | | || || | | |
             __  __      |_| \_\\__,_|_|_|_|___|_| |_|           _
            |  \/  | __ _(_)_ __ |_   _| |__  _ __ ___  __ _  __| |
            | |\/| |/ _` | | '_ \  | | | '_ \| '__/ _ \/ _` |/ _` |
            | |  | | (_| | | | | | | | | | | | | |  __/ (_| | (_| |
            |_|  |_|\__,_|_|_| |_| |_| |_| |_|_|  \___|\__,_|\__,_|
/* */

void FreshSiodThread ()  // make this idempotent (it's ok if thread is half dead)
{
	//
	// UNDONE: synchronization hole, here.  The SIOD thread could be in the 
	// process of suicide when this variable is tested.  
	//

	if (g_hSiodReplThread)
	{
		g_quitNow             = 1 ;
		g_NewSiodThreadNeeded = 1 ;

		// The following will tell SIOD thread to suicide and to create another
		// copy of itself.

		PulseEvent (g_charArrivedEvent) ;
	}
	else
	{
		CreateSiodThread () ;
	}
}


void ListenerOnChar (HWND hwnd, CHAR wParam, LPARAM lParam)
{
	if ((0 == g_hSiodReplThread) || wParam == '\012')
	{
		// ignore
	}

	else if (wParam == '\b')
	{
		if (g_getTail != 0xffffffff)
			g_getTail -- ;
	}
	else
	{
		if (wParam == '\r')
			wParam = '\n' ;

		g_getBuffer [++ g_getTail] = wParam ;

		if (wParam == '\n')
			PulseEvent (g_charArrivedEvent) ;
	}
}


LRESULT CALLBACK ListenerProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg) 
	{
	case WM_CHAR :
		ListenerOnChar (hwnd, wParam, lParam) ;
		break ;
	}

	return g_oldListenerWndPrc (hwnd, iMsg, wParam, lParam) ; ;
}

/////////////////////////////////////////////////////////////////////////////////
/* * /
                          ____              ___
                         |  _ \ _   _ _ __ |_ _|_ __
                         | |_) | | | | '_ \ | || '_ \
                         |  _ <| |_| | | | || || | | |
             ____ ___ ___|_|_\_\\__,_|_| |_|___|_| |_|          _
            / ___|_ _/ _ \|  _ \|_   _| |__  _ __ ___  __ _  __| |
            \___ \| | | | | | | | | | | '_ \| '__/ _ \/ _` |/ _` |
             ___) | | |_| | |_| | | | | | | | | |  __/ (_| | (_| |
            |____/___\___/|____/  |_| |_| |_|_|  \___|\__,_|\__,_|
/* */

DWORD WINAPI ThreadSiodRepl (LPVOID lpv)
{
	// When SiodMain returns, the SIOD thread is kaput.  It may, however, have
	// been asked by Main Threadto start up a new copy of SIOD.  Only SIOD thread 
	// knows when it's safe to start up a new SIOD, so, here goes.  There is no
	// race, because only this function will reset the variable 
	// g_NewSiodThreadNeeded, in SIOD Thread, and only Main Thread will
	// set this variable (down below in some UI code).  

	int r = SiodMain (0, NULL, NULL) ;

	CloseHandle (g_hSiodReplThread) ;

	g_hSiodReplThread = 0 ;

	if (g_NewSiodThreadNeeded)
	{
		g_NewSiodThreadNeeded = 0 ;

		CreateSiodThread () ;
	}

	return r ;
}

int _cdecl ListenerGetC (void *)
{
	if (g_ungetIndex)
	{
		return g_ungetBuffer [g_ungetIndex --] ;
	}
	else
	{
		if (g_getTail == g_getHead)
		{
			g_getTail = g_getHead = 0xffffffff ;
			WaitForSingleObject (g_charArrivedEvent, INFINITE) ;
		}

		// Fall through.  g_quitNow will be set if Main Thread has told
		// SIOD Thread to suicide.  There is no race because this variable is
		// only set in Main Thread and only reset in SIOD Thread.  

		if (g_quitNow)
		{
			g_quitNow = 0 ;

			quit () ;  // this should jmp to after SiodMain call site
		}

		int c = g_getBuffer [++ g_getHead] ;

		return c ;
	}
}

void _cdecl ListenerUngetC (int c, void *)
{
	g_ungetBuffer [++ g_ungetIndex] = c ;
}


static struct gen_readio g_gen_readio = 
{
	(int  (*)(void *)     ) ListenerGetC   , //  f_getc;
    (void (*)(int, void *)) ListenerUngetC , //  f_ungetc;
    NULL                                   ,
} ;


LISP _cdecl 
SiodLread(void)
{
    return (readtl(&g_gen_readio));
}


inline void newline ()
{
	Edit_ReplaceSel (g_hWndListener, "\r\012") ;
}


inline void PrintInListener()
{
	DWORD d = SendMessage (g_hWndListener, EM_REPLACESEL, 0L, (LPARAM)g_szTemp) ;
	if (d == 0)
		LastErrorBox () ;
}


void _cdecl SiodPuts (char * s)
{
	char * t = g_szTemp ;
	while (*s)
	{
		if (*s == '\n')
		{
			*t++ = '\r' ;
			*t++ = '\012' ;
		}
		else
			*t++ = *s ;

		s ++ ;
	}

	*t++ = 0 ;

	PrintInListener () ;
}

int _cdecl SiodIPuts (const char * s)
{
	SiodPuts ((char*)s) ;
	return 1 ;
}

int _cdecl SiodFPuts (char * s, void *)
{
	SiodPuts ((char*)s) ;
	return 1 ;
}


struct gen_printio g_gen_printio = 
{
	NULL, 
	SiodFPuts, 
	0
} ;


void _cdecl
SiodLprint(LISP exp)
{
    lprin1g(exp, &g_gen_printio);
    SiodPuts("\n");
}


static char siod_lib_arg[256] = "-l";
static char *library_filename = "libsiod.dll";
static char *registry_key = "SOFTWARE\\George Carrette\\SIOD";

static char *siod_argv[] =
{
  "siod",
  "-h100000:10",
  "-g0",
  "-o1000",
  "-s200000",
  "-n2048",
  siod_lib_arg
};

int SiodMain(int argc, char **argv, char **env)
{
    int nargc = argc;
    char **nargv = argv;
    DWORD vtype,vsize;
    long status;
    HKEY siodkey;


#if 0
#ifdef _DEBUG
        int tmpflag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

        tmpflag |= _CRTDBG_ALLOC_MEM_DF;
        tmpflag |= _CRTDBG_CHECK_ALWAYS_DF;
        tmpflag |= _CRTDBG_LEAK_CHECK_DF;

        _CrtSetDbgFlag (tmpflag);

        _CrtSetBreakAlloc(0x88);
#endif
#endif

	status = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			      registry_key,
			      0,
			      KEY_QUERY_VALUE,
			      &siodkey);
	if (status == ERROR_SUCCESS)
	  {
	    vsize = sizeof(siod_lib_arg) - 2;
	    status = RegQueryValueEx(siodkey,
				     "SIOD_LIB",
				     NULL,
				     &vtype,
				     (LPBYTE)&siod_lib_arg[2],
				     &vsize);
	    if (status == ERROR_SUCCESS)
	      {
		if (vtype != REG_SZ)
		  siod_lib_arg[2] = 0;
	      }
	    status = RegCloseKey(siodkey);
	  }

    if (nargc > 0)
      siod_shuffle_args(&nargc,&nargv);

    process_cla(sizeof(siod_argv)/sizeof(char*), siod_argv, 1);
	
    set_stdout_hooks (SiodPuts);
    set_repl_hooks (SiodPuts, SiodLread, 0, SiodLprint) ;

    int r = siod_main (nargc, nargv, env) ;

	return r ;
}


////////////////////////////////////////////////////////////////////////////////
/* * /                                                                    /* * /
       _                    _           _____ _                        _
      | |    ___   __ _  __| | ___ _ __|_   _| |__  _ __ ___  __ _  __| |
      | |   / _ \ / _` |/ _` |/ _ \ '__| | | | '_ \| '__/ _ \/ _` |/ _` |
      | |__| (_) | (_| | (_| |  __/ |    | | | | | | | |  __/ (_| | (_| |
      |_____\___/ \__,_|\__,_|\___|_|    |_| |_| |_|_|  \___|\__,_|\__,_|
/* * /                                                                     /* */


DWORD WINAPI ThreadLoader (LPVOID)
{
	sprintf (g_szTemp, "Unloading the bobble...");
	PrintInListener () ;

	return 1;
}


////////////////////////////////////////////////////////////////////////////////  
/* * /
                              _   _
                             | | | |___  ___ _ __
                             | | | / __|/ _ \ '__|
                             | |_| \__ \  __/ |
                   ___       _\___/|___/\___|_|
                  |_ _|_ __ | |_ ___ _ __ / _| __ _  ___ ___
                   | || '_ \| __/ _ \ '__| |_ / _` |/ __/ _ \
                   | || | | | ||  __/ |  |  _| (_| | (_|  __/
                __|___|_| |_|\__\___|_|  |_|  \__,_|\___\___|
               | __ )  ___ (_) | ___ _ __ _ __ | | __ _| |_ ___
               |  _ \ / _ \| | |/ _ \ '__| '_ \| |/ _` | __/ _ \
               | |_) | (_) | | |  __/ |  | |_) | | (_| | ||  __/
               |____/ \___/|_|_|\___|_|  | .__/|_|\__,_|\__\___|
                                         |_|
/* */

#define CONTROLS_Y_OFFSET 80

#define EDIT_X_OFFSET   11
#define EDIT_Y_OFFSET    4
#define EDIT_Y_HEIGHT   20
#define EDIT_Y_WHOLE    (2 * EDIT_Y_OFFSET + EDIT_Y_HEIGHT)

#define LISTENER_X_OFFSET   0
#define LISTENER_Y_OFFSET
#define LISTENER_Y_HEIGHT 200
#define LISTENER_Y_WHOLE

void DlgOnSize (HWND hwnd, UINT, int cx, int cy) 
{
	SetWindowPos
	(
		GetDlgItem(hwnd, IDC_EDIT_MODULE_FILENAME), 
		NULL, 
		EDIT_X_OFFSET, 
		EDIT_Y_OFFSET,
		cx-2*EDIT_X_OFFSET, 
		EDIT_Y_OFFSET + EDIT_Y_HEIGHT, 
		SWP_NOZORDER
	);

	SetWindowPos
	(
		GetDlgItem(hwnd, IDC_EDIT_LISTENER), 
		NULL, 
		LISTENER_X_OFFSET, 
		CONTROLS_Y_OFFSET + EDIT_Y_HEIGHT,
		cx, 
		cy-CONTROLS_Y_OFFSET - EDIT_Y_HEIGHT, 
		SWP_NOZORDER
	);
}


BOOL DlgOnInitDialog (HWND hwnd, HWND, LPARAM) 
{
	g_hWndListener  = GetDlgItem (hwnd, IDC_EDIT_LISTENER) ;
	g_hWndEdit      = GetDlgItem (hwnd, IDC_EDIT_MODULE_FILENAME);

	g_oldListenerWndPrc = (WNDPROC) 
		SetWindowLong (g_hWndListener, GWL_WNDPROC, (LONG)ListenerProc) ;

	RECT rc;
	
	//
	// Associate an icon with the dialog box.
	//
    SendMessage
	(
		hwnd, 
		WM_SETICON, 
		TRUE,  
		(LPARAM)LoadIcon
		(
			GetWindowInstance(hwnd), 
			MAKEINTRESOURCE(IDI_DLGHUSK)
		)
	); 
    
	SendMessage
	(
		hwnd, 
		WM_SETICON, 
		FALSE, 
		(LPARAM)LoadIcon
		(
			GetWindowInstance(hwnd), 
			MAKEINTRESOURCE(IDI_DLGHUSK)
		)
	); 
	
	//
	// The list box must be sized first because the system
	// doesn't send a WM_SIZE message to the dialog box when
	// it's first created.
	//
	GetClientRect(hwnd, &rc);

	DlgOnSize (hwnd, 0, rc.right, rc.bottom) ;

	CreateSiodThread () ;

	return(TRUE);
}


char szClipData [4096 * 100] ;

void CopyControlToClipboard (HWND hwnd) 
{
	/* * /
	int     nCount                  ;
	int     nNum                    ;
	CHAR    szLine      [1000]      ;
	HGLOBAL hClipData               ;
	LPSTR   lpClipData              ;
	BOOL    fOk                     ;

	nCount = ListBox_GetCount(hwnd);
	
	for( nNum = 0; nNum < nCount; nNum++ )
	{
		ListBox_GetText(hwnd, nNum, szLine);
		strcat(szClipData, szLine);
		strcat(szClipData, "\r\n");
	}        

	OpenClipboard(NULL); 
	EmptyClipboard();

	// Clipboard accepts only data that is in a block allocated 
	// with GlobalAlloc using the GMEM_MOVEABLE and 
	// GMEM_DDESHARE flags.

	hClipData = GlobalAlloc
	(
		GMEM_MOVEABLE | GMEM_DDESHARE, 
		sizeof(CHAR) * (strlen(szClipData) + 1)
	);
	
	lpClipData = (LPSTR) GlobalLock(hClipData);

	strcpy(lpClipData, szClipData);

	fOk = (SetClipboardData(CF_TEXT, hClipData) == hClipData);

	CloseClipboard();

	if( !fOk )
	{
		GlobalFree(hClipData);
		MessageBoxA
		(
			GetFocus(),
			"Error putting text on the clipboard", 
			NULL, MB_OK | MB_ICONINFORMATION
		);
	}
	/* */
}



/////////////////////////////////////////////////////////////


CHAR szFilters[] = { "Modules (*.dll;*.exe)\0*.DLL;*.EXE\0All Files (*.*)\0*.*\0\0" } ;


void DlgOnCommand (HWND hwnd, int id, HWND, UINT) 
{
	OPENFILENAME ofn;

	switch (id) 
	{
		/* * /
	case IDC_BUTTON_COPY:
		CopyControlToClipboard (g_hWndListBox) ;
		break ;
		/* */

    case IDC_BUTTON_BROWSE:
		
		ZERO_STRUCT(ofn, TRUE);
        
		ofn.hwndOwner    = hwnd;
        ofn.lpstrFile    = szVictimModule;
        ofn.nMaxFile     = _MAX_PATH;
        ofn.lpstrTitle   = "Select module (.dll or .exe) to analyze & load";
		ofn.lpstrFilter  = szFilters;
        ofn.Flags        = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_READONLY;
      
		GetOpenFileNameA (&ofn);
        
		if (stricmp (ofn.lpstrFile, szOriginalVictimModule) == 0)
			strcpy (ofn.lpstrFile, szOriginalShortVictim) ;

		SetDlgItemText (hwnd, IDC_EDIT_MODULE_FILENAME, ofn.lpstrFile);

		SetFocus (GetDlgItem (hwnd, IDC_BUTTON_LOAD));
        
		break;

	case IDC_BUTTON_FRESH_SIOD:

		FreshSiodThread () ;

		break;

	case IDC_BUTTON_LOAD:

		Edit_SetSel     (g_hWndEdit, 0, -1) ;
		Edit_ReplaceSel (g_hWndEdit, ""   ) ;

		GetDlgItemText (hwnd, IDC_EDIT_MODULE_FILENAME, szVictimModule, _MAX_PATH);

		g_hLoaderThread = CreateThread
		(
			NULL,							// No Security Attributes, please
			0,								// Default stack size, please
			ThreadLoader, 					// Run this function, please
			0,								// Function dosn't need any input
			0,								// Wake up now 
			&g_dwLoaderThreadId				// Must have an ID on Win95
		);

		if (g_hLoaderThread == NULL)
			LastErrorBox();

		break;
	
	case IDCANCEL:

		if (g_hSiodReplThread)
			CloseHandle (g_hSiodReplThread);

		if (g_hLoaderThread)
			CloseHandle (g_hLoaderThread);

		EndDialog(hwnd, id);

		break;
	}
}


/////////////////////////////////////////////////////////////

	
BOOL CALLBACK DlgProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	g_hDialog = hwnd ;

#if 0
#ifdef _DEBUG
    {
        char a [128] ;
        sprintf (a, "hWnd from DlgProc: 0x%08x\n", hwnd) ;
        OutputDebugStringA (a) ;
    }
#endif
#endif

	switch (uMsg) 
	{
	HANDLE_DLGMSG(hwnd, WM_INITDIALOG, DlgOnInitDialog);
	HANDLE_DLGMSG(hwnd, WM_COMMAND,    DlgOnCommand);
	HANDLE_DLGMSG(hwnd, WM_SIZE,	   DlgOnSize);

	/* * /
	case WM_APP:
		DlgOnApp (hwnd, wParam, lParam) ;
		return TRUE ;
		break ;
	/* */
	}

	return(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
/* * / 

                   __        ___       __  __       _
                   \ \      / (_)_ __ |  \/  | __ _(_)_ __
                    \ \ /\ / /| | '_ \| |\/| |/ _` | | '_ \
                     \ V  V / | | | | | |  | | (_| | | | | |
                      \_/\_/  |_|_| |_|_|  |_|\__,_|_|_| |_|
/* */

int WINAPI _tWinMain 
(
	 HINSTANCE hinstExe
	,HINSTANCE //hinstPrev
	,LPTSTR    //pszCmdLine
	,int       //nCmdShow
) 
{
	InitEvents () ;

	DialogBox
	(
		hinstExe, 
		MAKEINTRESOURCE(IDD_DLGHUSK), 
		NULL, 
		(DLGPROC)DlgProc // WEIRD: I didn't used to have to cast this to DLGPROC
	);
	return(0);
}


/////////////////////////////////////////////////////////////////////////////
/* * /
                   _             _    _____
                  | |_   _ _ __ | | _|  ___|   _ _ __   ___ ___
               _  | | | | | '_ \| |/ / |_ | | | | '_ \ / __/ __|
              | |_| | |_| | | | |   <|  _|| |_| | | | | (__\__ \
               \___/ \__,_|_| |_|_|\_\_|   \__,_|_| |_|\___|___/
/* */

void InitEvents ()
{
	g_charArrivedEvent  = CreateEvent (0, FALSE, FALSE, 0) ;
}


DWORD LastErrorBox()
{
	char * pszMessage;
    DWORD e = GetLastError () ;
	FormatMessage
	(	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		0, e, 0, (LPSTR)(&pszMessage),	0, 0
	);
	MessageBox (NULL, pszMessage, "Error", MB_OK);
	LocalFree (pszMessage);
    return e ;
}

            
double DRand()
{
	return ( double(rand()) / RAND_MAX );
}


DWORD DwRand (DWORD dwMax)
{
#pragma warning (disable:4244) // don't worry about downconversion, thank you.
	return DRand() * dwMax;
#pragma warning (default:4244)
}


DWORD DwGaussianRand (DWORD dwMean, DWORD dwStdDev)
{
	DWORD dwZeroMean;

	dwZeroMean  = DwRand (dwStdDev / 2);
	dwZeroMean += DwRand (dwStdDev / 2);
	dwZeroMean += DwRand (dwStdDev / 2);
	dwZeroMean += DwRand (dwStdDev / 2);
	dwZeroMean += DwRand (dwStdDev / 2);

	dwZeroMean /= 5;

	return dwZeroMean + dwMean;
}



char LoChar (WORD w)
{
	char c = ( (char*) (&w) )[0] ;
	if (isalnum(c))
		return c;
	else
		return '.';
}


char HiChar (WORD w)
{
	char c = ( (char*) (&w) )[1] ;
	if (isalnum(c))
		return c;
	else
		return '.';
}

char CharOfDw (int iWhich, DWORD dw)
{
	char c = ( (char*) (&dw) )[iWhich] ;
	if (isalnum(c))
		return c;
	else
		return '.';
}

/////////////////////////////////////////////////////////////////////////////
/* * /
                  _____           _  ___   __ _____ _ _
                 | ____|_ __   __| |/ _ \ / _|  ___(_) | ___
                 |  _| | '_ \ / _` | | | | |_| |_  | | |/ _ \
                 | |___| | | | (_| | |_| |  _|  _| | | |  __/
                 |_____|_| |_|\__,_|\___/|_| |_|   |_|_|\___|
/* */
