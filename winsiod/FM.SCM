;;; From Brian Beckman's siodffi.zip distribution.

(require 'syslibs.scm)
(define user32 (syslib "user32.dll"))

(define (mb string . optional-title)
  (MessageBoxA 0 string
               (if optional-title
                   (car optional-title)
                   "Message Box")
               0))

;;;
;;; Can be loaded only when kernel32 and msvcrt40 are already in memory
;;;
;;; TODO: (UNSOLVED MYSTERIES) This doesn't work when I tell the system
;;;                            to allocate the message buffer.  I tried
;;;       every combination, even leaking the buffers (see below).  I 
;;;       did not analyze the call site instructions for Scheme vs. C++ 
;;;       (I have a version of this function that works fine in C++).  
;;;       I also did not try to find out whether there's some initialization 
;;;       of the memory system that makes this fail.
;;;
;;;       (OUT-OF-DATE):
;;;       This leaks the message, because the call to LocalFree fails, and
;;;       seems to fail only in this context.  I have tried multiple calls 
;;;       to LocalAlloc and LocalFree, interleaved, leaking, and repeated, 
;;;       and all behaves as expected.  ONLY in this circumstance does the 
;;;       call fail.  On failure, LocalFree returns the address freed, and
;;;       so does GetLastError, so there is no visibility.  Free also fails
;;;       to free the buffer (with an ACCVIO).  The behavior is the same on
;;;       NT and Win9x, and the behavior is NOT exhibited in an equivalent
;;;       C program.  The behavior appears with both VC5 and VC6, so it 
;;;       does not seem to be a compiler code-generation problem.  I have
;;;       verified that the C stack is correct before and after all calls
;;;       (checked the dispatchers in realload.cpp).  I have not gotten to 
;;;       the bottom of this and I'm out of ideas, so I'm punting it for
;;;       now (24 May 1998)
;;;

(define (FormatMessage code)
  (let* ((buf (LocalAlloc #x40 128))
         (ret (FormatMessageA #x1000 0 code 0 buf 127 0))
         (str (cstring->string buf)))
    (LocalFree buf)
    str))

(define (ChkHandleReturn handle)
  (if (= handle 0)
      (begin
        (print "!@#^%$ !@#^%$ -------- E R R O R -------- $%^#@! $%^#@!")
        (let ((e (GetLastError)))
          (prin1 "Error code: ")
          (print e)
          (print (FormatMessage e)))))
  handle)

(define (MBChkHandleReturn handle)
  (if (= handle 0)
    (MessageBoxA 0 (FormatMessage (GetLastError)) "Null Handle Unexpected" 0))
  handle)

(define (ChkSysCall actual . expected)
  (let ((exp (if (null? expected) 0 (car expected))))
    (if (not (= actual exp))
        (begin
          (print "!@#^%$ !@#^%$ -------- E R R O R -------- $%^#@! $%^#@!")
          (print (FormatMessage (GetLastError))))))
  actual)

(define (ChkHresult hres)
  (if (not (= 0 (bit-and #x80000000 hres)))
      (begin
        (print "!@#^%$ !@#^%$ -------- E R R O R -------- $%^#@! $%^#@!")
        (print (FormatMessage hres))))
  hres)

(define (MBChkSysCall actual . expected)
  (let ((exp (if (null? expected) 0 (car expected))))
    (if (not (= actual exp)
        (MessageBoxA 0 (FormatMessage (GetLastError))
                     "System Call Error" 0))))
  actual)

(define (MBChkHresult hres)
  (if (not (= 0 (bit-and #x80000000 hres)))
      (MessageBoxA 0 (FormatMessage hres) "Unexpected HRESULT" 0))
  hres)
  
