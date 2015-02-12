(define (spawn-torch x y)
  (spawn-lightsource (make-lightsource (+ x 16) (+ y 16) 300 1 (make-color 0.7 0.7 0.5 1))) (spawn-fx (make-fx 1 (make-color 0.1 0.1 0.1 1) (+ x 16) y 8 50 100)))

(define (spawn-outside-torch x y)
  (spawn-lightsource (make-lightsource (+ x 16) (+ y 16) 200 1 (make-color 0.7 0.7 0.5 1))) (spawn-fx (make-fx 1 (make-color 0.1 0.1 0.1 1) (+ x 16) y 8 50 100)))
