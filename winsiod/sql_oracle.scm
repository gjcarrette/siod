;;-*-mode:lisp-*-
;; For use with the ORACLE CALL INTERFACE (OCI) interface to SIOD.
;;
;; siod -g0 -h150000 -isql_oracle.scm
;;
;; Procedures: (oracle-sql-init "username" "password")
;;             (oracle-sql "string") => result of operation.
;;             (oracle-show-tables) => list user tables.
;;
;; Note: This only scratches the surface. There is much more
;;       functionality available at lisp level that is provided
;;       by the sql_oracle.c module than is utilized here.
;;       Contact the author if you need to do something more sophisticated,
;;       like deal with parameterized sql statements. (Or read the C code).
;; $Id: sql_oracle.scm,v 1.1 1996/10/17 18:40:18 gjc Exp $
;;

(define *oracle-sql-username* "system")
(define *oracle-sql-password* "manager")
(define *oracle-sql-association* nil)

(define (oracle-sql-init . args)
  (if (null? *oracle-sql-association*)
      (begin (oracle-login
	      (or (car args) *oracle-sql-username*)
	      (if (cdr args) (car (cdr args)) *oracle-sql-password*))
	     (set! *oracle-sql-association* t))))

(define (unwind-protected l1 l2)
  (let ((x (*catch 'errobj (l1))))
    (l2)
    x))

(define (oracle-sql str)
  (oracle-sql-init)
  (let ((s nil)
	(n nil)
	(j 0)
	(result nil)
	(tmp nil))
    (unwind-protected
     (lambda ()
       (set! s (oracle-sql-prepare str))
       (set! n (oracle-nselects s))
       (oracle-execute s)
       (if (> n 0)
	   (begin (while (< j n)
		    (set! tmp (cons (oracle-select-column-name s j) tmp))
		    (set! j (+ 1 j)))
		  (set! result (cons (nreverse tmp) nil))
		  (while (oracle-fetch s)
		    (set! j 0)
		    (set! tmp nil)
		    (while (< j n)
		      (set! tmp (cons (oracle-select-column-value s j) tmp))
		      (set! j (+ 1 j)))
		    (set! result (cons (nreverse tmp) result)))
		  (set! result (nreverse result))))
       result)
     (lambda ()
       (if s (oracle-sql-release s))))))

(define (oracle-show-tables)
  (oracle-sql "select * from user_catalog"))
