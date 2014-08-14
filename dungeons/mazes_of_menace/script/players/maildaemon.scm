(set-player-item 1 (make-item "laser"))
(set-player-item 2 (make-item "flamethrower"))

(define (determine-beam-dims xv yv)
  (if (= xv 0)
    (values 8 16)
    (values 16 8)))

;(set-player-item 2 (make-item "sfx/beam.wav"
;                             (lambda (i data)
;                               (let ((c (get-item-charge i)))
;                                (if (is-item-active i)
;                                  (if (> c 2)
;                                    (begin
;                                      (call-with-values (lambda () (determine-beam-dims (get-item-xv i) (get-item-yv i)))
;                                                        (lambda (width height)
;                                                          (spawn-projectile (make-color 1 1 1 1)
;                                                                            (+ (get-item-x i) 12) (get-item-y i)
;                                                                            (* (get-item-xv i) 16) (* (get-item-yv i) 16)
;                                                                            width height
;                                                                            100
;                                                                            i
;                                                                            1)))
;                                      (set-item-charge i (- c 2)))
;                                    (deactivate-item i))
;                                  (if (<= c 100)
;                                    (set-item-charge i (+ c 1))))))))


(set-player-item-index 1)
