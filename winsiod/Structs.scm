;;; From Brian Beckman's siodffi.zip distribution.

(require 'siod.scm)
(require 'syslibs.scm)

;;; "slots" is a list.  Each element of the list is either a symbol
;;; or a pair.  If a symbol, then the size of the named slot is 
;;; assumed to be 4.  If a pair, then the car of the pair must be 
;;; a symbol, naming the slot, and the cdr (not the cadr) of the pair
;;; must be an int containing the size of the slot. 

(define (struct tag slots)

  (define (scoped-sym sym)
    (if (not (symbol? tag))
        (error "wta to scoped-sym" tag))
    (symbolconc tag ':: sym))

  (define (slot-name slot)
    (cond
     ((symbol? slot) slot)
     ((pair?   slot) (let ((nym (car slot)))
                       (if (symbol? nym) nym
                           (error "(car slot) must be a symbol" nym))))
     (#t (error "wta to slot-name" slot))))

  (define (slot-size slot)
    (cond
     ((symbol? slot) 4)
     ((pair?   slot) (let ((syz (cdr slot)))
                       (if (number? syz) syz
                           (error "(cdr slot) must be a number" syz))))
     (#t (error "wta to slot-size" slot))))

  (define (total-sizeof slots)
    (define (size-so-far accum slots)
      (cond
       ((null? slots) accum)
       (#t (size-so-far (+ accum (slot-size (car slots))) (cdr slots)))))
    (size-so-far 0 slots))

  ;; The following routines bind makers and nukers for the struct
  ;; at-large, and setters and getters for each of the struct's slots.

  (define (bind-struct-sizeof)
    (let ((sym2 (symbolconc 'sizeof- tag))
          (syz  (total-sizeof slots)))
      (print sym2)
      (set-symbol-value!
        sym2
        (lambda () syz))))

  (define (bind-struct-maker)
    (let ((sym2 (symbolconc 'new- tag)))
      (print sym2)
      (set-symbol-value!
       sym2
       (lambda () (LocalAlloc #x40 (total-sizeof slots))))))

  (define (bind-struct-array-maker)
    (let ((sym2 (symbolconc 'new-rg- tag)))
      (print sym2)
      (set-symbol-value!
       sym2
       (lambda (n) (LocalAlloc #x40 (* n (total-sizeof slots)))))))

  (define (bind-struct-nuker)
    (let ((sym2 (symbolconc 'del- tag)))
      (print sym2)
      (set-symbol-value!
       sym2
       (lambda (instance) (LocalFree instance)))))

  (define (bind-struct-array-nuker)
    (let ((sym2 (symbolconc 'del-rg- tag)))
      (print sym2)
      (set-symbol-value!
       sym2
       (lambda (array) (LocalFree array)))))

  (define (make-bind-xxx-slot-getter machine-datum-peeker)
    (lambda (slotNym offset)
      (let ((sym2 (scoped-sym (symbolconc 'get- slotNym))))
        (print sym2)
        (set-symbol-value!
          sym2
          (lambda (struct)
            (machine-datum-peeker (+ struct offset)))))))

  (define bind-dword-slot-getter
    (make-bind-xxx-slot-getter peek-dword))
    
  (define bind-word-slot-getter
    (make-bind-xxx-slot-getter peek-word))
    
  (define bind-byte-slot-getter
    (make-bind-xxx-slot-getter peek-byte))
    
  (define (make-bind-xxx-slot-setter machine-datum-poker)
    (lambda (slotNym offset)
      (let ((sym2 (scoped-sym (symbolconc 'set- slotNym))))
        (print sym2)
        (set-symbol-value!
          sym2
          (lambda (struct datum)
            (machine-datum-poker (+ struct offset) datum))))))
  
  (define bind-dword-slot-setter
    (make-bind-xxx-slot-setter poke-dword))

  (define bind-word-slot-setter
    (make-bind-xxx-slot-setter poke-word))

  (define bind-byte-slot-setter
    (make-bind-xxx-slot-setter poke-byte))

  ;; If the size of the slot is 4, 2, or 1, then 
  ;; a slot getter is a (lambda (struct) ...) that returns the native 
  ;; value of the slot, and the slot setter is a (lambda (struct d) ...)
  ;; that sets the slot to the native value supplied in the argument
  ;; list.  Otherwise -- if the size of the slot is not 4, 2, or 1
  ;; -- then the slot getter is a (lambda (destination-address struct)
  ;; ...) that copies the slot data out from the struct, and the slot 
  ;; setter is a (lambda (struct source-address) ...), which copies 
  ;; the data into the struct. 
  
  (define (bind-arbitrary-peeker slotNym offset)
    (let ((sym2 (scoped-sym (symbolconc 'getAddr- slotNym))))
      (print sym2)
      (set-symbol-value!
       sym2
       (lambda (struct)
         (+ struct offset)))))

  (define (bind-arbitrary-getter slotNym offset syz)
    (let ((sym2 (scoped-sym (symbolconc 'get- slotNym))))
      (print sym2)
      (set-symbol-value!
       sym2
       (lambda (destination-address struct)
         (memcpy destination-address (+ struct offset) syz)))))

  (define (bind-arbitrary-setter slotNym offset syz)
    (let ((sym2 (scoped-sym (symbolconc 'set- slotNym))))
      (print sym2)
      (set-symbol-value!
       sym2
       (lambda (struct source-address)
         (memcpy (+ struct offset) source-address syz)))))

  (define (bind-slot-getter-setters slots offset)
    (cond
     ( (null? slots) offset )
     ( t (let* ((slot (car slots))
                (syz  (slot-size slot))
                (nym  (slot-name slot)))
           (cond
             ((= syz 4) (bind-dword-slot-getter nym offset)
                        (bind-dword-slot-setter nym offset))
             ((= syz 2) (bind-word-slot-getter nym offset)
                        (bind-word-slot-setter nym offset))
             ((= syz 1) (bind-byte-slot-getter nym offset)
                        (bind-byte-slot-setter nym offset))
             (#t        (bind-arbitrary-getter nym offset syz)
                        (bind-arbitrary-setter nym offset syz)
                        (bind-arbitrary-peeker nym offset syz)))
           (bind-slot-getter-setters (cdr slots) (+ offset syz))) )))

  (define (bind-struct-dumper)

    (define (getters-list)
      (map
       (lambda (slot) (scoped-sym (symbolconc 'get- (slot-name slot))))
       slots))

    (let ((sym2 (symbolconc 'dump- tag)))
      (print sym2)
      (set-symbol-value!
       sym2
       (lambda (struct)
         (prin1 "Instance of ...")
         (print tag)
         (map (lambda (getter slot)
                (let ((name (slot-name slot))
                      (size (slot-size slot)))
                  (prin1 name) (prin1 '=)
                  (if (or (= size 4) (= size 2) (= size 1))
                      (print ((eval getter) struct))
                      (let ((thyng (LocalAlloc #x40 size)))
                        ((eval getter) thyng struct)
                        (print (cstring->string thyng))
                        (print (cbytes->array thyng size))
                        (LocalFree thyng)))))
                (getters-list)
                slots)))))

  (bind-struct-maker)
  (bind-struct-nuker)
  (bind-struct-array-maker)
  (bind-struct-array-nuker)
  (bind-struct-sizeof)
  (bind-struct-dumper)
  (bind-slot-getter-setters slots 0))



