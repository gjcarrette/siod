;;-*-mode:lisp-*-
;; Name:    find-files.scm
;; Purpose: create lists of file directory trees,
;;          with status information and md5 checksums.
;;  
;; $Id: find-files.scm,v 1.3 1998/02/20 12:52:13 gjc Exp $

(cond ((eq? 'win32 (os-classification))
       (define file-status stat)
       (define (opendir-arg x) (string-append x "/*")))
      ('else
       (define file-status lstat)
       (define (opendir-arg x) x)))

(define (list-files dir)
  (if (> (verbose) 1)
      (writes nil dir "\n"))
  (let ((s (*catch 'errobj (opendir (opendir-arg dir))))
        (result nil)
	(f nil))
    (cond ((and (pair? s) (string? (car s)))
	   (print (cons dir s))
	   nil)
	  ('else
	   (while (set! f (readdir s))
	     (set! result (cons f result)))
	   (closedir s)
	   (nreverse result)))))

(define (find-files start)
  (let ((l (list-files start))
	(result nil)
	(f nil)
	(s nil))
    (while l
      (if (not (equal? (car l) ".."))
	  (begin (set! f (string-append start "/" (car l)))
		 (set! s (file-status f))
		 (set! result (cons (cons f s) result))
		 (if (and (memq 'DIR (assq 'mode s))
			  (not (equal? (car l) ".")))
		     (set! result (nconc (nreverse (find-files f)) result)))))
      (set! l (cdr l)))
    (nreverse result)))

(define (get-file-info l)
  ;; used on the result of a find-files.
  (let ((mode (assq 'mode l)))
    (cond ((memq 'LNK mode)
	   (let ((result (*catch 'errobj (readlink (car l)))))
	     (if (string? result)
		 (set-cdr! l
			   (cons (cons 'readlink result)
				 (cdr l))))))
	  ((memq 'REG mode)
	   (let ((f nil)
		 (c (md5-init))
		 (b (cons-array 4096 'string)))
	     (*catch 'errobj
		     (begin (set! f (fopen (car l) "rb"))
			    (md5-update c b f)
			    (set-cdr! l
				      (cons (cons 'md5
						  (array->hexstr
						   (md5-final c)))
					    (cdr l)))))
	     (and f (fclose f))))))
  l)

(define (get-directory-snapshot dir)
  (mapcar get-file-info
	  (find-files dir)))

(define (snapshot save-file dir)
  (save-forms save-file (get-directory-snapshot dir)))
    
    

(define (get-filesystem-snapshots directories)
  (let ((result nil)
	(l directories)
	(s nil))
    (while l
      (set! s (file-status (car l)))
      (cond ((not s))
	    ((memq 'DIR (assq 'mode s))
	     (set! result (nconc result (get-directory-snapshot (car l)))))
	    ('else
	     (set! result (nconc result
				 (list (get-file-info (cons (car l) s)))))))
      (set! l (cdr l)))
    result))

