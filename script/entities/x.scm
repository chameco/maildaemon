(define GRID_BUG (build-entity-prototype "grid_bug" 32 32 10 8 2))

(set-entity-init
  GRID_BUG
  (lambda (e)
    (list (cons (get-entity-x e) (get-entity-y e)) 0 0)))

(set-entity-collide
  GRID_BUG
  (lambda (e data)
    (if (= (cadr data) 0)
      (begin
        (hit-player 10)
        (set-entity-xv e (- (get-entity-xv e)))
        (set-entity-yv e (- (get-entity-yv e)))
        (list (cons (get-entity-x e) (get-entity-y e)) 5 (caddr data)))
      data)))

(set-entity-update
  GRID_BUG
  (lambda (e data)
    (let ((curpos (cons (get-entity-x e) (get-entity-y e)))
          (t (get-entity-texture e)))
     (if (zero? (caddr data))
       (set-sheet-column t (modulo (+ (get-sheet-column t) 1) 3)))
     (if (and (equal? curpos (car data)) (= (cadr data) 0))
       (begin
         (set-entity-xv e (- (get-entity-xv e)))
         (set-entity-yv e (- (get-entity-yv e)))
         (list curpos (cadr data) (modulo (+ (caddr data) 1) 3)))
       (list curpos (max (- (cadr data) 1) 0) (modulo (+ (caddr data) 1) 3))))))
