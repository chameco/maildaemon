(build-item-prototype "laser"
                      (lambda (i data)
                        (let ((c (get-item-charge i)))
                         (if (is-item-active i)
                           (if (> c 5)
                             (begin
                               (spawn-projectile (make-projectile "laser" (+ (get-item-x i) 12) (get-item-y i) (get-item-rotation i) i))
                               (set-item-charge i (- c 5))
                               (deactivate-item i)))
                           (if (<= c 100) (set-item-charge i (+ c 1)))))))
