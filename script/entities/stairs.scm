(define STAIRS (build-entity-prototype "stairs" 32 32 0 0 0))
(set-entity-hit STAIRS (lambda (e dmg data) data))

(define (make-stairs dest x y)
  (let ([s (make-entity "stairs" x y)])
   (set-entity-collide s (lambda (e data) (switch-level dest)))
   s))
