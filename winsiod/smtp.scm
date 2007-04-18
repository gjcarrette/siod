;;-*-mode:lisp-*-
;; $Id: smtp.scm,v 1.2 1998/03/10 17:45:05 gjc Exp $

(require-so (so-ext "ss"))
(require-so (so-ext "regex"))

(define (send-data-via-smtp server to from header data dbg)
  (let ((s nil)
	(out nil)
	(in nil)
	(exp (regcomp "^([0-9]+)(\-| |$)(.*)$" REG_EXTENDED)))
    (set! s (s-open server "smtp"))
    (set! out (lambda strl
		(mapcar (lambda (str)
			  (cond (dbg
				 (writes nil str)
				 (fflush)))
			  (s-puts str s))
			strl)))
    (set! in (lambda ()
	       (s-force-output s)
	       (let ((match nil)
		     (line nil)
		     (code nil)
		     (continue t))
		 (while (and continue (set! line (s-gets s)))
		   (cond (dbg
			  (writes nil line)
			  (fflush)))
		   (cond ((pair? (set! match (regexec exp line)))
			  (set! code (string->number
				      (substring line
						 (car (nth 1 match))
						 (cdr (nth 1 match)))))
			  (set! continue (equal? "-"
						 (substring
						  line
						  (car (nth 2 match))
						  (cdr (nth 2 match))))))))
		 code)))
    (define (check-code c expect command)
      (let ((msd (and c (trunc (/ c 100)))))
	(cond ((not (equal? expect msd))
	       (error (string-append (if msd "error" "no")
				     " response from "
				     command
				     (if c
					 (string-append " code "
							(number->string c))
				       "")))))))
    (check-code (in) 2 "initial connection")
    (out "HELO " (gethostname) "\r\n")
    (check-code (in) 2 "hello command")
    (out "MAIL FROM: <" from ">\r\n")
    (check-code (in) 2 "from command")
    (out "RCPT TO: <" to ">\r\n")
    (check-code (in) 2 "recipient command")
    (out "DATA\r\n")
    (check-code (in) 3 "message data start")
    (cond ((eq? header 'generate)
	   (out "To: <" to ">\r\n"
		"From: <" from ">\r\n"
		"Date: " (http-date) "\r\n"
		"\r\n"))
	  ((not header))
	  ('else
	   (mapcar (lambda (line)
		     (out line "\r\n"))
		   header)
	   (out "\r\n")))
    (mapcar (lambda (line)
	      (out (if (equal? line ".") ".." line))
	      (out "\r\n"))
	    data)
    (out ".\r\n")
    (check-code (in) 2 "message data end")
    (out "QUIT\r\n")
    (in)
    (s-close s)))

(define (send-data-via-smtp-test-1)
  (send-data-via-smtp "ray.delphi.com"
		      "gjc@delphi.com"
		      "gjc@delphi.com"
		      'generate
		      '("this is"
			"a test")
		      t))
