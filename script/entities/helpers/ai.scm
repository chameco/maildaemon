(define (within a b margin) (< (abs (- a b)) margin))

(define (sign x) (if (< x 0) -1 (if (> x 0) 1 0)))

(define (generate-update-track-player frames time)
  (lambda (e data)
    (if (is-unbroken-line (get-entity-x e) (get-entity-y e) (get-player-x) (get-player-y))
      (let ((xdiff (- (get-player-x) (get-entity-x e)))
            (ydiff (- (get-player-y) (get-entity-y e)))
            (theta (calculate-angle (get-entity-x e) (get-entity-y e) (get-player-x) (get-player-y))))
        (set-entity-xv e (* (sign xdiff) (abs (cos theta)) (get-entity-speed e)))
        (set-entity-yv e (* (sign ydiff) (abs (sin theta)) (get-entity-speed e)))
        (if (within (abs xdiff) 0 (get-entity-speed e))
          (begin
            (set-entity-xv e 0)
            (set-entity-yv e (* (sign ydiff) (get-entity-speed e)))))
        (if (within (abs ydiff) 0 (get-entity-speed e))
          (begin
            (set-entity-xv e (* (sign xdiff) (get-entity-speed e)))
            (set-entity-yv e 0))))
      (begin
        (set-entity-xv e 0)
        (set-entity-yv e 0)))
    (let ((t (get-entity-texture e)))
     (if (zero? data)
       (set-sheet-column t (modulo (+ (get-sheet-column t) 1) frames))))
    (modulo (+ data 1) time)))
