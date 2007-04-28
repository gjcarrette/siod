;;;From Brian Beckman's siodffi.zip distribution.

(require "gdi32.scm")
(require "winuser.scm")
(require 'fm.scm)

(define (WndProc hWnd msg wParam lParam)
  (cond

   ((= msg WM_DESTROY)
    (PostQuitMessage 0))

   ((= msg WM_PAINT)
    (let* ((ps  (new-PAINTSTRUCT))
           (hDC (BeginPaint hWnd ps)))
      (TextOutA hDC 10 10 (string->cstring "Hello, World!") 13)
      (EndPaint hWnd ps)
      0))

   (#t (DefWindowProcA hWnd msg wParam lParam))) )

(define (myCreateWindow)
  (let ((wc      (new-WNDCLASS))
        (clsNym  (string->cstring "GenericAppClass"))
        (appNym  (string->cstring "Generic Application"))
        (clsStyl (| CS_OWNDC (| CS_VREDRAW CS_HREDRAW)))
        (winStyl (| WS_OVERLAPPEDWINDOW (| WS_VISIBLE (| WS_HSCROLL WS_VSCROLL))))
        (hInst   #x400000)
        (msg     (new-MSG))
        ;;(mnuNym  (string->cstring "GenericAppMenu"))
        ;;(icon    (LoadIconA 0 101))
        ;;(cursor  (LoadCursorA 0 104))
        )

    (WNDCLASS::set-lpszClassName wc clsNym)
    (WNDCLASS::set-lpfnWndProc   wc (callback WndProc ))
    (WNDCLASS::set-style         wc clsStyl)
    (WNDCLASS::set-hInstance     wc hInst)
    ;;(WNDCLASS::set-lpszMenuName  wc mnuNym)
    ;;(WNDCLASS::set-hIcon         wc icon)
    ;;(WNDCLASS::set-hCursor       wc cursor)
    (WNDCLASS::set-hbrBackground wc 6)

    (ChkHandleReturn (RegisterClassA wc))
    (ChkHandleReturn (CreateWindowExA 0 clsNym appNym winStyl
                                      0 0 CW_USEDEFAULT CW_USEDEFAULT 
                                      0 0 hInst 0))

    (while (GetMessageA msg 0 0 0)
           (TranslateMessage msg)
           (DispatchMessageA msg)) ))
       
(define (test)
  (load 'windows.scm)
  (let ((wh (myCreateWindow)))
    (prin1 "Window Handle: ")
    (print wh) ))

