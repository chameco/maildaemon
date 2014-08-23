(define SIGN (build-entity-prototype "sign" 32 32 0 0 0))
(set-entity-hit SIGN (lambda (e dmg data) data))

(define (make-sign x y text)
  (let ([s (make-entity "sign" x (- y 16))])
   (set-entity-collide s (lambda (e data) (set-current-dialog text)))
   s))
