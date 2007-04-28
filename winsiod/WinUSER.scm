;;;
;;; From Brian Beckman's siodffi.zip distribution.

(require 'syslibs.scm)
(require 'structs.scm)
(require "winbase.scm")

;;; typedef struct _WNDCLASS { 
;;;     UINT       style; 
;;;     WNDPROC    lpfnWndProc; 
;;;     int        cbClsExtra; 
;;;     int        cbWndExtra; 
;;;     HINSTANCE  hInstance; 
;;;     HICON      hIcon; 
;;;     HCURSOR    hCursor; 
;;;     HBRUSH     hbrBackground; 
;;;     LPCTSTR    lpszMenuName; 
;;;     LPCTSTR    lpszClassName; 
;;; } WNDCLASS, *PWNDCLASS; 

(define WNDCLASS
  (struct 'WNDCLASS
    `(style     lpfnWndProc cbClsExtra cbWndExtra
      hInstance hIcon       hCursor    hbrBackground
      lpszMenuName lpszClassName)))

(define CS_VREDRAW          #x0001)
(define CS_HREDRAW          #x0002)
(define CS_DBLCLKS          #x0008)
(define CS_OWNDC            #x0020)
(define CS_CLASSDC          #x0040)
(define CS_PARENTDC         #x0080)
(define CS_NOCLOSE          #x0200)
(define CS_SAVEBITS         #x0800)
(define CS_BYTEALIGNCLIENT  #x1000)
(define CS_BYTEALIGNWINDOW  #x2000)
(define CS_GLOBALCLASS      #x4000)

(define CS_IME              #x00010000)

; /*
;  * Window Styles
;  */
(define WS_OVERLAPPED       #x00000000)
(define WS_POPUP            #x80000000)
(define WS_CHILD            #x40000000)
(define WS_MINIMIZE         #x20000000)
(define WS_VISIBLE          #x10000000)
(define WS_DISABLED         #x08000000)
(define WS_CLIPSIBLINGS     #x04000000)
(define WS_CLIPCHILDREN     #x02000000)
(define WS_MAXIMIZE         #x01000000)
(define WS_CAPTION          #x00C00000)    ; /* WS_BORDER | WS_DLGFRAME  */
(define WS_BORDER           #x00800000)
(define WS_DLGFRAME         #x00400000)
(define WS_VSCROLL          #x00200000)
(define WS_HSCROLL          #x00100000)
(define WS_SYSMENU          #x00080000)
(define WS_THICKFRAME       #x00040000)
(define WS_GROUP            #x00020000)
(define WS_TABSTOP          #x00010000)

(define WS_MINIMIZEBOX      #x00020000)
(define WS_MAXIMIZEBOX      #x00010000)


; /*
;  * Common Window Styles
;  */
(define WS_OVERLAPPEDWINDOW (| WS_OVERLAPPED     
                            (| WS_CAPTION        
                            (| WS_SYSMENU        
                            (| WS_THICKFRAME     
                            (| WS_MINIMIZEBOX    
                               WS_MAXIMIZEBOX ))))))
                                                 
(define WS_POPUPWINDOW      (| WS_POPUP          
                            (| WS_BORDER         
                               WS_SYSMENU )))

(define WS_CHILDWINDOW      WS_CHILD)

(define WS_TILED            WS_OVERLAPPED)
(define WS_ICONIC           WS_MINIMIZE)
(define WS_SIZEBOX          WS_THICKFRAME)
(define WS_TILEDWINDOW      WS_OVERLAPPEDWINDOW)

; /*
;  * Extended Window Styles
;  */
(define WS_EX_DLGMODALFRAME     #x00000001)
(define WS_EX_NOPARENTNOTIFY    #x00000004)
(define WS_EX_TOPMOST           #x00000008)
(define WS_EX_ACCEPTFILES       #x00000010)
(define WS_EX_TRANSPARENT       #x00000020)
; #if(WINVER >= 0x0400)
(define WS_EX_MDICHILD          #x00000040)
(define WS_EX_TOOLWINDOW        #x00000080)
(define WS_EX_WINDOWEDGE        #x00000100)
(define WS_EX_CLIENTEDGE        #x00000200)
(define WS_EX_CONTEXTHELP       #x00000400)

(define WS_EX_RIGHT             #x00001000)
(define WS_EX_LEFT              #x00000000)
(define WS_EX_RTLREADING        #x00002000)
(define WS_EX_LTRREADING        #x00000000)
(define WS_EX_LEFTSCROLLBAR     #x00004000)
(define WS_EX_RIGHTSCROLLBAR    #x00000000)

(define WS_EX_CONTROLPARENT     #x00010000)
(define WS_EX_STATICEDGE        #x00020000)
(define WS_EX_APPWINDOW         #x00040000)


(define WS_EX_OVERLAPPEDWINDOW  (| WS_EX_WINDOWEDGE WS_EX_CLIENTEDGE))
(define WS_EX_PALETTEWINDOW     (| WS_EX_WINDOWEDGE
                                (| WS_EX_TOOLWINDOW WS_EX_TOPMOST)))

(define WM_NULL                         #x0000)
(define WM_CREATE                       #x0001)
(define WM_DESTROY                      #x0002)
(define WM_MOVE                         #x0003)
(define WM_SIZE                         #x0005)
(define WM_ACTIVATE                     #x0006)
;/*
; * WM_ACTIVATE state values
; */
(define     WA_INACTIVE     0)
(define     WA_ACTIVE       1)
(define     WA_CLICKACTIVE  2)

(define WM_SETFOCUS                     #x0007)
(define WM_KILLFOCUS                    #x0008)
(define WM_ENABLE                       #x000A)
(define WM_SETREDRAW                    #x000B)
(define WM_SETTEXT                      #x000C)
(define WM_GETTEXT                      #x000D)
(define WM_GETTEXTLENGTH                #x000E)
(define WM_PAINT                        #x000F)
(define WM_CLOSE                        #x0010)
(define WM_QUERYENDSESSION              #x0011)
(define WM_QUERYOPEN                    #x0013)
(define WM_ENDSESSION                   #x0016)
(define WM_QUIT                         #x0012)
(define WM_ERASEBKGND                   #x0014)
(define WM_SYSCOLORCHANGE               #x0015)
(define WM_SHOWWINDOW                   #x0018)
(define WM_WININICHANGE                 #x001A)
(define WM_SETTINGCHANGE                WM_WININICHANGE)

(define WM_DEVMODECHANGE                #x001B)
(define WM_ACTIVATEAPP                  #x001C)
(define WM_FONTCHANGE                   #x001D)
(define WM_TIMECHANGE                   #x001E)
(define WM_CANCELMODE                   #x001F)
(define WM_SETCURSOR                    #x0020)
(define WM_MOUSEACTIVATE                #x0021)
(define WM_CHILDACTIVATE                #x0022)
(define WM_QUEUESYNC                    #x0023)

(define WM_GETMINMAXINFO                #x0024)
;*
;* Struct pointed to by WM_GETMINMAXINFO lParam
;*/
;typedef struct tagMINMAXINFO {
;    POINT ptReserved;
;    POINT ptMaxSize;
;    POINT ptMaxPosition;
;    POINT ptMinTrackSize;
;    POINT ptMaxTrackSize;
;} MINMAXINFO, *PMINMAXINFO, *LPMINMAXINFO;

(define WM_PAINTICON                    #x0026)
(define WM_ICONERASEBKGND               #x0027)
(define WM_NEXTDLGCTL                   #x0028)
(define WM_SPOOLERSTATUS                #x002A)
(define WM_DRAWITEM                     #x002B)
(define WM_MEASUREITEM                  #x002C)
(define WM_DELETEITEM                   #x002D)
(define WM_VKEYTOITEM                   #x002E)
(define WM_CHARTOITEM                   #x002F)
(define WM_SETFONT                      #x0030)
(define WM_GETFONT                      #x0031)
(define WM_SETHOTKEY                    #x0032)
(define WM_GETHOTKEY                    #x0033)
(define WM_QUERYDRAGICON                #x0037)
(define WM_COMPAREITEM                  #x0039)
(define WM_GETOBJECT                    #x003D)
(define WM_COMPACTING                   #x0041)
(define WM_WINDOWPOSCHANGING            #x0046)
(define WM_WINDOWPOSCHANGED             #x0047)

(define WM_NOTIFY                       #x004E)
(define WM_INPUTLANGCHANGEREQUEST       #x0050)
(define WM_INPUTLANGCHANGE              #x0051)
(define WM_TCARD                        #x0052)
(define WM_HELP                         #x0053)
(define WM_USERCHANGED                  #x0054)
(define WM_NOTIFYFORMAT                 #x0055)

(define NFR_ANSI                             1)
(define NFR_UNICODE                          2)
(define NF_QUERY                             3)
(define NF_REQUERY                           4)

(define WM_CONTEXTMENU                  #x007B)
(define WM_STYLECHANGING                #x007C)
(define WM_STYLECHANGED                 #x007D)
(define WM_DISPLAYCHANGE                #x007E)
(define WM_GETICON                      #x007F)
(define WM_SETICON                      #x0080)

(define WM_NCCREATE                     #x0081)
(define WM_NCDESTROY                    #x0082)
(define WM_NCCALCSIZE                   #x0083)
(define WM_NCHITTEST                    #x0084)
(define WM_NCPAINT                      #x0085)
(define WM_NCACTIVATE                   #x0086)
(define WM_GETDLGCODE                   #x0087)
(define WM_SYNCPAINT                    #x0088)
(define WM_NCMOUSEMOVE                  #x00A0)
(define WM_NCLBUTTONDOWN                #x00A1)
(define WM_NCLBUTTONUP                  #x00A2)
(define WM_NCLBUTTONDBLCLK              #x00A3)
(define WM_NCRBUTTONDOWN                #x00A4)
(define WM_NCRBUTTONUP                  #x00A5)
(define WM_NCRBUTTONDBLCLK              #x00A6)
(define WM_NCMBUTTONDOWN                #x00A7)
(define WM_NCMBUTTONUP                  #x00A8)
(define WM_NCMBUTTONDBLCLK              #x00A9)

(define WM_KEYFIRST                     #x0100)
(define WM_KEYDOWN                      #x0100)
(define WM_KEYUP                        #x0101)
(define WM_CHAR                         #x0102)
(define WM_DEADCHAR                     #x0103)
(define WM_SYSKEYDOWN                   #x0104)
(define WM_SYSKEYUP                     #x0105)
(define WM_SYSCHAR                      #x0106)
(define WM_SYSDEADCHAR                  #x0107)
(define WM_KEYLAST                      #x0108)

(define WM_IME_STARTCOMPOSITION         #x010D)
(define WM_IME_ENDCOMPOSITION           #x010E)
(define WM_IME_COMPOSITION              #x010F)
(define WM_IME_KEYLAST                  #x010F)

(define WM_INITDIALOG                   #x0110)
(define WM_COMMAND                      #x0111)
(define WM_SYSCOMMAND                   #x0112)
(define WM_TIMER                        #x0113)
(define WM_HSCROLL                      #x0114)
(define WM_VSCROLL                      #x0115)
(define WM_INITMENU                     #x0116)
(define WM_INITMENUPOPUP                #x0117)
(define WM_MENUSELECT                   #x011F)
(define WM_MENUCHAR                     #x0120)
(define WM_ENTERIDLE                    #x0121)

(define WM_MDICREATE                    #x0220)
(define WM_MDIDESTROY                   #x0221)
(define WM_MDIACTIVATE                  #x0222)
(define WM_MDIRESTORE                   #x0223)
(define WM_MDINEXT                      #x0224)
(define WM_MDIMAXIMIZE                  #x0225)
(define WM_MDITILE                      #x0226)
(define WM_MDICASCADE                   #x0227)
(define WM_MDIICONARRANGE               #x0228)
(define WM_MDIGETACTIVE                 #x0229)

(define WM_MDISETMENU                   #x0230)
(define WM_ENTERSIZEMOVE                #x0231)
(define WM_EXITSIZEMOVE                 #x0232)
(define WM_DROPFILES                    #x0233)
(define WM_MDIREFRESHMENU               #x0234)

(define WM_IME_SETCONTEXT               #x0281)
(define WM_IME_NOTIFY                   #x0282)
(define WM_IME_CONTROL                  #x0283)
(define WM_IME_COMPOSITIONFULL          #x0284)
(define WM_IME_SELECT                   #x0285)
(define WM_IME_CHAR                     #x0286)

(define WM_CUT                          #x0300)
(define WM_COPY                         #x0301)
(define WM_PASTE                        #x0302)
(define WM_CLEAR                        #x0303)
(define WM_UNDO                         #x0304)
(define WM_RENDERFORMAT                 #x0305)
(define WM_RENDERALLFORMATS             #x0306)
(define WM_DESTROYCLIPBOARD             #x0307)
(define WM_DRAWCLIPBOARD                #x0308)
(define WM_PAINTCLIPBOARD               #x0309)
(define WM_VSCROLLCLIPBOARD             #x030A)
(define WM_SIZECLIPBOARD                #x030B)
(define WM_ASKCBFORMATNAME              #x030C)
(define WM_CHANGECBCHAIN                #x030D)
(define WM_HSCROLLCLIPBOARD             #x030E)
(define WM_QUERYNEWPALETTE              #x030F)
(define WM_PALETTEISCHANGING            #x0310)
(define WM_PALETTECHANGED               #x0311)
(define WM_HOTKEY                       #x0312)

(define CW_USEDEFAULT           #x80000000)


;/*
; * Message structure
; */
;typedef struct tagMSG {
;    HWND        hwnd;
;    UINT        message;
;    WPARAM      wParam;
;    LPARAM      lParam;
;    DWORD       time;
;    POINT       pt;
;#ifdef _MAC
;    DWORD       lPrivate;
;#endif
;} MSG, *PMSG, NEAR *NPMSG, FAR *LPMSG;

(define MSG
  (struct 'MSG
          '(hwnd message wParam lParam time (pt . 8) lPrivate)))

;typedef struct tagPAINTSTRUCT {
;    HDC         hdc;
;    BOOL        fErase;
;    RECT        rcPaint;
;    BOOL        fRestore;
;    BOOL        fIncUpdate;
;    BYTE        rgbReserved[32];
;} PAINTSTRUCT, *PPAINTSTRUCT, *NPPAINTSTRUCT, *LPPAINTSTRUCT;

(define PAINTSTRUCT
  (struct 'PAINTSTRUCT
          `(hdc fErase (rcPaint . 16)
                fRestore fIncUpdate (rgbReserved . 32))))

;/*
; * ShowWindow() Commands
; */
(define SW_HIDE             0)
(define SW_SHOWNORMAL       1)
(define SW_NORMAL           1)
(define SW_SHOWMINIMIZED    2)
(define SW_SHOWMAXIMIZED    3)
(define SW_MAXIMIZE         3)
(define SW_SHOWNOACTIVATE   4)
(define SW_SHOW             5)
(define SW_MINIMIZE         6)
(define SW_SHOWMINNOACTIVE  7)
(define SW_SHOWNA           8)
(define SW_RESTORE          9)
(define SW_SHOWDEFAULT      10)
(define SW_FORCEMINIMIZE    11)
(define SW_MAX              11)

;/*
; * Old ShowWindow() Commands
; */
(define HIDE_WINDOW         0)
(define SHOW_OPENWINDOW     1)
(define SHOW_ICONWINDOW     2)
(define SHOW_FULLSCREEN     3)
(define SHOW_OPENNOACTIVATE 4)

;/*
; * Identifiers for the WM_SHOWWINDOW message
; */
(define SW_PARENTCLOSING    1)
(define SW_OTHERZOOM        2)
(define SW_PARENTOPENING    3)
(define SW_OTHERUNZOOM      4)


