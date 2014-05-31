(set-player-weapon 1 (make-weapon "sfx/laser.wav"
                                  (lambda (w data)
                                    (let ((c (get-weapon-charge w)))
                                     (if (is-weapon-firing w)
                                       (if (> c 10)
                                         (begin
                                           (spawn-projectile (make-color 1 0 0 1)
                                                             (+ (get-weapon-x w) 12) (get-weapon-y w)
                                                             (* (get-weapon-xv w) 16) (* (get-weapon-yv w) 16)
                                                             8 8
                                                             100
                                                             w
                                                             20)
                                           (set-weapon-charge w (- c 10))
                                           (release-trigger w)))
                                       (if (<= c 100) (set-weapon-charge w (+ c 1))))))))

(define (determine-beam-dims xv yv)
  (if (= xv 0)
    (values 8 16)
    (values 16 8)))

(set-player-weapon 2 (make-weapon "sfx/beam.wav"
                                  (lambda (w data)
                                    (let ((c (get-weapon-charge w)))
                                     (if (is-weapon-firing w)
                                       (if (> c 2)
                                         (begin
                                           (call-with-values (lambda () (determine-beam-dims (get-weapon-xv w) (get-weapon-yv w)))
                                                             (lambda (width height)
                                                               (spawn-projectile (make-color 1 1 1 1)
                                                                                 (+ (get-weapon-x w) 12) (get-weapon-y w)
                                                                                 (* (get-weapon-xv w) 16) (* (get-weapon-yv w) 16)
                                                                                 width height
                                                                                 100
                                                                                 w
                                                                                 1)))
                                           (set-weapon-charge w (- c 2)))
                                         (release-trigger w))
                                       (if (<= c 100)
                                         (set-weapon-charge w (+ c 1))))))))
                                         

(set-player-weapon-index 1)
