;; See the file sql_msql.c for more information.
;; This code was sent to me by rwieland@ralf.zalf.de
;; $Id: sql_msql.scm,v 1.1 1996/10/17 18:40:18 gjc Exp $

(require-so (so-ext 'sql_msql))

;;
;;
;; Examples for use of msql2siod

;; sql.scm
;; some definitions for easier use
(set! *handle* nil)
(define (msql-select x) (set! *handle* (msql-query x)))
(define (msql-fetch) (msql-fetch-row *handle*))
(define (msql-seek n) (msql-data-seek *handle* n))
(define (msql-free) (msql-free-result *handle*)(set! *handle* nil))
(define (msql-nfields) (msql-numfields *handle*))
(define (msql-nrows) (msql-numrows *handle*))
(define (msql-change x) (msql-update x)(set! *handle* nil))
;;
;; it is a simple example of my research
;; 
;; Mainprogram
;; Datastructure:
;; #
;; # mSQL Dump  (requires mSQL-1.0.6 or better)
;; #
;; # Host: localhost    Database: se
;; #--------------------------------------------------------


;; #
;; # Table structure for table 'habitat'
;; #
;; CREATE TABLE habitat (
;;   vertex CHAR(15) NOT NULL PRIMARY KEY,
;;   leaf1  CHAR(15),
;;   leaf2  CHAR(15),
;;   leaf3  CHAR(15),
;;   leaf4  CHAR(15),
;;   prog   CHAR(15)
;; ) \g


;; #
;; # Dumping data for table 'habitat'
;; #
;; 
;; INSERT INTO habitat VALUES ('habitat','nutriens','reproduction','danger','','mhabitat')\g
;; INSERT INTO habitat VALUES ('reproduction','nisting_place','staying_place','','','mfortpf')\g
;; INSERT INTO habitat VALUES ('nisting_place','quiet','safe','protected','','mnistpl')\g
;; INSERT INTO habitat VALUES ('staying_place','quiet','safe','protected','','mnistpl')\g
;; INSERT INTO habitat VALUES ('nutriens','quality','distance','characteristic','','mnahr')\g
;; INSERT INTO habitat VALUES ('characteristic','structure','ways','','','mmerkmale')\g
;; INSERT INTO habitat VALUES ('quality','grassland','grain','other','','mqualitaet')\g
;; INSERT INTO habitat VALUES ('danger','road','rail','winter','','mfeinde')\g

;; #
;; # Table structure for table 'werte'
;; #
;; CREATE TABLE werte (
;;   vertex CHAR(15) NOT NULL PRIMARY KEY,
;;   min REAL,
;;   max REAL
;; ) \g


;; #
;; # Dumping data for table 'werte'
;; #

;; INSERT INTO werte VALUES ('habitat',0,1)\g
;; INSERT INTO werte VALUES ('reproduction',0,0.7)\g
;; INSERT INTO werte VALUES ('danger',0,0.5)\g
;; INSERT INTO werte VALUES ('nutriens',0,0.7)\g
;; INSERT INTO werte VALUES ('nisting_place',0.2,0.8)\g
;; INSERT INTO werte VALUES ('staying_place',0.2,0.5)\g
;; INSERT INTO werte VALUES ('quality',0.2,0.6)\g
;; INSERT INTO werte VALUES ('distance',0.3,0.9)\g
;; INSERT INTO werte VALUES ('charakteristic',0,0.8)\g
;; INSERT INTO werte VALUES ('structur',0.2,0.8)\g
;; INSERT INTO werte VALUES ('ways',0,0.5)\g
;; INSERT INTO werte VALUES ('quiet',0.2,0.75)\g
;; INSERT INTO werte VALUES ('safe',0.3,0.85)\g
;; INSERT INTO werte VALUES ('protected',0.2,0.7)\g
;; INSERT INTO werte VALUES ('rail',0.2,0.9)\g
;; INSERT INTO werte VALUES ('road',0.2,0.7)\g
;; INSERT INTO werte VALUES ('winter',0.3,0.75)\g


(msql-open "ralf" "seng")
(msql-select "SELECT vertex, leaf1,leaf2,leaf3,leaf4,prog from habitat where vertex='nisting_place'")
(set! fp (fopen "test.sql" "w"))
(set! question "select vertex,min,max from werte where vertex='")
(set! leafes (msql-fetch))
(msql-free)
(while (and (car leafes) (< 0 (string-length (string-trim (car leafes)))))
       (set! question1 (string-append question (car leafes)))
       (set! question2 (string-append question1 "'"))
       (set! leafes (cdr leafes))
       (msql-select question2)
       (while (set! erg (msql-fetch))
              (while (car erg)
                    (fwrite (car erg) fp)
                    (fwrite "     " fp)
                    (set! erg (cdr erg)))
                    (fwrite "\n" fp))
        )
(fclose fp)
(msql-close)
(quit)


;; After you have load the database you can start the example with
;;
;; siod -iselect.scm
;;
;; (select.scm means the little siod program in this text.)
;; "se" is the name of the database you had created,
;; "ralf" means the local hostname.
;; The program produce the file test.sql as result.
;; it must look like that :-)
;;
;; nistingplace     0.2     0.8     
;; quite     0.2     0.75     
;; protected     0.3     0.85     
;;
;; many luck with the program.
