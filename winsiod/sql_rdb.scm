;;-*-mode:lisp-*-
;; For use with the DIGITAL RDB SQL SERVICES interface to SIOD.
;; 20-JAN-94 GJC.
;;
;; Loading (into siod linked with sql_rdb.obj)
;;  $siod -g0 -isql_rdb.scm -h150000
;;
;; Procedures: (rdb-sql-init "database-name")
;;             (rdb-sql-error?) => last sql error
;;             (rdb-sql "string") => result of operation.
;;             (rdb-show-table "table-name") => column information.
;;             (rdb-show-tables) => list all tables.
;;
;; Note: This only scratches the surface. There is much more
;;       functionality available at lisp level that is provided
;;       by the sql_rdb.c module than is utilized here.
;;       Contact the author if you need to do something more sophisticated,
;;       like deal with parameterized sql statements. (Or read the C code).
;; $Id: sql_rdb.scm,v 1.1 1996/10/17 18:40:18 gjc Exp $

(define *rdb-sql-association* nil)
(define *rdb-sql-database* nil)

(define (rdb-sql-init db)
  (if (null? *rdb-sql-association*)
      (begin (set! *rdb-sql-association* (rdb-sql-associate))
	     (if db
		 (set! *rdb-sql-database* db))
	     (if *rdb-sql-database*
		 (rdb-sql-execute-immediate
		  *rdb-sql-association*
		  (string-append "declare schema filename "
				 *rdb-sql-database*))))))

(define (rdb-sql-error?)
  (rdb-sql-error-buffer *rdb-sql-association*))

(define (rdb-sql-cleanup release-associations?)
  (let ((l (rdb-sql-associations)))
    (while l
      (let ((s (rdb-sql-association-statements (car l))))
	(while s
	  (rdb-sql-release (car s))
	  (set! s (cdr s))))
      (if release-associations?
	  (rdb-sql-release (car l)))
      (set! l (cdr l)))))

(define (unwind-protected l1 l2)
  (let ((x (*catch 'errobj (l1))))
    (l2)
    x))

(define (rdb-sql cmd)
  (rdb-sql-init nil)
  (let ((s nil)
	(p nil)
	(l nil)
	(c nil)
	(result nil)
	(row nil)
	(rowp nil)
	(x nil))
    (unwind-protected
     (lambda ()
       (set! s (rdb-sql-prepare-cached *rdb-sql-association* cmd))
       (set! p (rdb-sql-statement-params s))
       (set! l (rdb-sql-statement-selects s))
       (if p (error "params not implemented"))
       (if (null? l)
	   (set! result (rdb-sql-execute s))
	 (begin (rdb-sql-declare-cursor s 'table 'read-only)
		(set! c (rdb-sql-open-cursor s))
		(while (rdb-sql-fetch s)
		  (set! rowp l)
		  (set! row nil)
		  (while rowp
		    (set! row (cons (rdb-sql-get-column s
							(car (cdr (car rowp))))
				    row))
		    (set! rowp (cdr rowp)))
		  (set! result (cons (nreverse row) result)))
		(set! rowp l)
		(set! row nil)
		(while rowp
		  (set! row (cons (car (car rowp)) row))
		  (set! rowp (cdr rowp)))
		(set! result (cons (nreverse row) (nreverse result)))))
       result)
     (lambda ()
       (if c (rdb-sql-close-cursor s))
       (if s (rdb-sql-release-cached s))))))

(define rdb-sql-prepare-cached rdb-sql-prepare)
(define rdb-sql-release-cached rdb-sql-release)

(define (rdb-show-tables)
  (rdb-sql "select rdb$relation_name,rdb$system_flag from rdb$relations"))

(define (rdb-show-table x)
  (let ((s nil)
	(l nil))
    (unwind-protected
     (lambda ()
       (set! s (rdb-sql-prepare *rdb-sql-association*
				(string-append "select * from " x)))
       (rdb-describe-statement s)
       (set! l (rdb-sql-statement-selects s)))
     (lambda ()
       (and s (rdb-sql-release s))))))
