;; name: setup.scm
;; purpose: set windows registry to point to the current
;;  directory as the SIOD_LIB

(define (main)
 (let ((l (siod-lib))
       (key "SOFTWARE\\George Carrette\\SIOD")
       (r "HKEY_LOCAL_MACHINE")
       (v "SIOD_LIB"))

 (writes nil "Setting " r "\\" key "\\" v " = " l "\n")
 (require-so (so-ext 'slibw32))
 (registry-set r key v l)))


