(build-item-prototype "laser"
                      (lambda (i data)
                        (let ((c (get-item-charge i)))
                         (if (is-item-active i)
                           (if (> c 5)
                             (begin
                               (spawn-projectile (make-color 1 0 0 1)
                                                 (+ (get-item-x i) 12) (get-item-y i)
                                                 16
                                                 (get-item-rotation i)
                                                 8 8
                                                 100
                                                 i
                                                 20)
                               (set-item-charge i (- c 5))
                               (deactivate-item i)))
                           (if (<= c 100) (set-item-charge i (+ c 1)))))))
