(define STAIRS (build-entity-prototype "stairs" 32 32 0 0 0))
(set-entity-hit STAIRS (lambda (e dmg data) data))

(define (make-stairs x y sr sc dest destx desty)
  (let ([s (make-entity "stairs" x y)])
   (set-entity-collide s (lambda (e data)
                           (begin
                             (switch-level dest)
                             (warp-player destx desty))))
   (set-sheet-row (get-entity-texture s) sr)
   (set-sheet-column (get-entity-texture s) sc)
   s))
