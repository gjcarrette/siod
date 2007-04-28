;;; From Brian Beckman's siodffi.zip distribution.
;;;    ____         ___ __
;;;   / __/_ _____ / (_) /  ___   _________ _
;;;  _\ \/ // (_-</ / / _ \(_-<_ (_-< __/  ' \
;;; /___/\_, /___/_/_/_.__/___(_)___|__/_/_/_/
;;;    _/___/   _           __          ___  ___     ___  ____    ______ ___ ___
;;;   / _ )____(_)__ ____  / / ___ ____/ _ \|_  |___/ _ \/ / /___<  / _ Y _ Y _ \
;;;  / _  / __/ / _ `/ _ \/ _ Y -_) __/ // / __/___/ // /_  _/___/ /\_, |_, |_, /
;;; /____/_/ /_/\_,_/_//_/_.__|__/\__/\___/____/   \___/ /_/    /_//___/___/___/
;;; 
;;; SIOD Interface to Windows system libraries. 
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
;;;

(require 'peloader.scm)

(define (syslib foo)
  (LoadLibrary (sysdir foo)))

;;; (define rcprt4   (syslib  "rpcrt4.dll"  ))

(define ole32    (syslib  "ole32.dll"   ))
(define oleAut32 (syslib  "oleaut32.dll"))
(define k32      (syslib  "kernel32.dll"))

;;; warning!  puts and putc get overwritten, and it may cause problems.
(define crt      (syslib  "msvcrt40.dll"))

