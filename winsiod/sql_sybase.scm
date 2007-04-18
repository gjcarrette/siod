;;-*-mode:lisp-*-
;; $Id: sql_sybase.scm,v 1.1 1996/10/17 18:40:18 gjc Exp $
;;
;; The (rpc "procedure_name") routine is convenient for interactive
;; debugging of database programs.

(if (not (symbol-bound? 'sybase-open))
    (require-so (so-ext 'sql_sybase)))

(define *sybase-status-table* (cons-array 20))

(hset *sybase-status-table* 0 "OK")
(hset *sybase-status-table* 1 "ERROR")

(define (print-sybase-result result)
  (mapcar
   (lambda (x)
     (if (and (pair? x) (eqv? 'CS_ROW_RESULT (car x)))
	 (let ((columns (cadr x))
	       (row-n (if (cdr (cddr x)) 1)))
	   (mapcar (lambda (row)
		     (if row-n
			 (begin (writes nil
					"----ROW " row-n " ----\n")
				(set! row-n (+ 1 row-n))))
		     (let ((n (length columns))
			   (j 0))
		       (while (< j n)
			 (writes nil
				 (aref columns j)
				 " = "
				 (or (aref row j)
				     "**NULL**")
				 "\n")
			 (set! j (+ 1 j)))))
		   (cddr x)))
       (if (and (pair? x) (eqv? 'CS_STATUS_RESULT (car x)))
	   (writes nil "STATUS = "
		   (or (href *sybase-status-table*
			     (aref (car (cddr x)) 0))
		       (aref (car (cddr x)) 0))
		   "\n")
	 (print x)))
     (writes nil "-------------\n"))
   (if (pair? result) result (list result))))

(define (rpc . args)
  (writes nil "*** " (car args) " ***\n")
  (let ((result (apply sybase-execute
		       (cons (car args)
			     (cons CS_RPC_CMD (cdr args))))))
    (print-sybase-result result)
    result))

(define *pads* (cons-array 100))

(define (padstr n)
  (or (href *pads* n)
      (hset *pads* n (cons-array n 'string))))

(define (format-row data w)
  (let ((j 0)
	(datum nil))
    (while (< j (length data))
      (set! datum (aref data j))
      (cond ((not (car w)))
	    ((number? datum)
	     (writes nil (number->string datum nil (abs (car w)))))
	    ((not datum)
	     (writes nil (padstr (abs (car w)))))
	    ((and (symbol? datum)
		  (set! datum (string-append datum))
		  nil))
	    ((<= (length datum) (abs (car w)))
	     (writes nil
		     (padstr (- (abs (car w)) (length datum)))
		     datum))
	    ((< (car w) 0)
	     (writes nil
		     (substring datum 0 (abs (car w)))))
	    ('else
	     (writes nil datum)))
      (cond ((< (+ 1 j) (length data))
	     (if (car w) (writes nil " ")))
	    ('else
	     (writes nil "\n")))
      (set! j (+ 1 j))
      (if (cdr w)
	  (set! w (cdr w))))))

(define (format-rows rows widths)
  (while rows
    (format-row (car rows) widths)
    (set! rows (cdr rows))))
