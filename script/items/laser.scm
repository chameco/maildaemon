(build-item-prototype "laser"
                      (lambda (i data)
                        (let ((c (get-item-charge i)))
                         (if (is-item-active i)
                           (if (> c 10)
                             (begin
                               (spawn-projectile (make-color 1 0 0 1)
                                                 (+ (get-item-x i) 12) (get-item-y i)
                                                 (* (get-item-xv i) 16) (* (get-item-yv i) 16)
                                                 8 8
                                                 100
                                                 i
                                                 20)
                               (set-item-charge i (- c 10))
                               (deactivate-item i)))
                           (if (<= c 100) (set-item-charge i (+ c 1)))))))
