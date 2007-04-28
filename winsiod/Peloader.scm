;;; From Brian Beckman's siodffi.zip distribution.
;;;    ___  ______              __
;;;   / _ \/ __/ / ___ ___ ____/ /__ ____  _________ _
;;;  / ___/ _// /_/ _ Y _ `/ _  / -_) __/ (_-< __/  ' \
;;; /_/__/___/____|___|_,_/\_,_/\__/_/ (_)___|__/_/_/_/_  ____    ______ ___ ___
;;;   / _ )____(_)__ ____  / / ___ ____/ _ \|_  |___/ _ \/ / /___<  / _ Y _ Y _ \
;;;  / _  / __/ / _ `/ _ \/ _ Y -_) __/ // / __/___/ // /_  _/___/ /\_, |_, |_, /
;;; /____/_/ /_/\_,_/_//_/_.__|__/\__/\___/____/   \___/ /_/    /_//___/___/___/
;;; 
;;; Interface to brianbec's FFI (foreign-function interface) for SIOD.
;;; 

;;;  ____
;;; | __ ) _   _  __ _ ___
;;; |  _ \| | | |/ _` / __|
;;; | |_) | |_| | (_| \__ \
;;; |____/ \__,_|\__, |___/
;;;              |___/
;;; 
;;; Once syslibs are loaded, floating-point function call like sin and pow are
;;; vectored off to Microsoft's C Runtime Library rather than to Siod's internal
;;; glue functions.  Since my FFI (foreign-function interface) currently converts
;;; all SIOD types to DWORDs before passing them to C, functionss like "sin" and
;;; "pow" won't work.  The proper solution is an integer type in SIOD so that my
;;; FFI can know that flonums should not be converted. 
;;;  _   _           _
;;; | | | |_ __   __| | ___  _ __   ___
;;; | | | | '_ \ / _` |/ _ \| '_ \ / _ \
;;; | |_| | | | | (_| | (_) | | | |  __/
;;;  \___/|_| |_|\__,_|\___/|_| |_|\___|
;;; 
;;;
;;; support for C++ objects and methods (moderately easy)
;;; support for callbacks written in scheme (easy)
;;; support for double-precision math functions (so-so)
;;;

;;;
;;; The RealLoad DLL implements GetProcAlist, MainDispatch, 
;;; MainDispatchCDecls, and cstring->string, which one would 
;;; use to convert 'flonums' returned by C functions into 
;;; strings Siod can deal with.
;;;

(require-so   "RealLoad.dll")
(set! **hex-mode** t)
(define (set-hex-mode)     (set! **hex-mode** t))
(define (reset-hex-mode)   (set! **hex-mode** '()))
(define (set-decimal-mode) (set! **hex-mode** '()))
(define (toggle-hex-mode)  (set! **hex-mode** (not **hex-mode**)))

;;
;; Some generic utility stuff for arrays of dwords
;; 

(define (iter-dwords cDw pTop arity1)
  (define (iter-help i)
    (cond ((>= i cDw) 'done)
          (t (arity1 (peek-dw (+ pTop (* i 4))))
             (iter-help (+ i 1)))))
  (iter-help 0))

(define (set-dw pTop offset dw)
  (poke-dw (+ pTop (* offset 4)) dw))

(define (get-dw pTop offset)
  (peek-dw (+ pTop (* offset 4))))
                      
;;
;; "setDispatch" is very important. It takes a function name, found from
;; enumerating the export table of a DLL, and an address, found by calling
;; Win32::GetProcAddress, and binds them into a new closure that you can 
;; call from Scheme. This closure calls MainDispatch, which takes a function
;; and an argument list, pushes all the arguments on the C stack, and 
;; calls the address as a __stdcall.  
;;

(define (setDispatch-careful-sym x)
  ;; this will prevent bashing siod "built-in" functions
  ;; in particular those which have the same name
  ;; as our favorites from the C runtime library.
  (cond ((not (symbol-bound? x))
	 x)
	((memq (typeof (symbol-value x))
	       '(tc_subr_0 tc_subr_1 tc_subr_2 tc_subr_2n tc_subr_3
			   tc_subr_4 tc_subr_5 tc_lsubr
			   tc_fsubr tc_msubr))
	 (symbolconc 'f: x))
	('else x)))

(define (setDispatch three)
  (let* ((sym   (setDispatch-careful-sym (car three)))
         (adr   (cadr three))
         (cdecl (cddr three))
         (disp  (if (= cdecl 1) MainDispatchCDecl MainDispatch)))
    (set-symbol-value! sym (lambda y (disp adr y)))
     sym))

(define (LoadLibrary dll)
  (mapcar setDispatch (GetProcAlist dll)))

(define (LoadLibraryCDecls dll)
  (mapcar setDispatch (GetProcAlistCDecls dll)))

(define (sysdir subdir)
  (string-append (ScmGetSystemDirectory) "/" subdir))

    



