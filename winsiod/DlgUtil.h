/******************************************************************************
Module name: DlgUtil.h
******************************************************************************/
#define _WIN32_WINNT 0x0400
#ifndef STRICT
#define STRICT
#endif // STRICT

///////////////////////////  HANDLE_DLGMSG Macro  /////////////////////////////

//
// The normal HANDLE_MSG macro in WINDOWSX.H does not work properly for dialog
// boxes because DlgProc return a BOOL instead of an LRESULT (like
// WndProcs). This HANDLE_DLGMSG macro corrects the problem:
//
#define HANDLE_DLGMSG(hwnd, message, fn)                   \
   case (message): return (SetDlgMsgResult(hwnd, uMsg,      \
      HANDLE_##message((hwnd), (wParam), (lParam), (fn))))


/////////////////////////// Quick MessageBox Macro ////////////////////////////


#define MB(s)                                       \
{                                                    \
	TCHAR szTMP[128];                                 \
	GetModuleFileName(NULL, szTMP, chDIMOF(szTMP));    \
	MessageBox(GetActiveWindow(), s, szTMP, MB_OK);     \
}


///////////////////////////////// End of File /////////////////////////////////


    //----------------------------------------------------------------
   //             
  // MISCELLANEOUS
 //
//----------------------------------------------------------------

#define outline // opposite of inline? -- means look in the cpp file for definition.

inline void DbgErrorBox (DWORD error)
{
#ifdef _DEBUG
	char * pszMessage;                                            
    FormatMessage                                                  
    (   FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        0, error, 0, (LPSTR)(&pszMessage), 0, 0);          
    MessageBox (NULL, pszMessage, "Error", MB_OK);                  
    LocalFree (pszMessage);   
#else
	error += 0 ;
#endif
}

inline void DbgLastErrorBox ()
{
	DbgErrorBox (GetLastError ()) ;
}

//
// Zero out a structure. If fInitSize is TRUE, initialize the first int to
// the size of the structure. Many structures like WNDCLASSEX and STARTUPINFO
// require that their first member be set to the size of the structure itself.
//
// DON'T USE THIS ON ANYTHING THAT HAS A VTABLE  !!! ! !  !  ! ! !!!
//
#define ZERO_STRUCT(S, F)               \
   (ZeroMemory(&(S), sizeof(S)), F ? (((DWORD*) &(S))[0] = sizeof(S)) : 0)


#define C_ARRAY_ELTS(A) (sizeof(A) / sizeof(A[0]))


