(build-item-prototype "sword"
                      (lambda (i data)
                        (let ((c (get-item-charge i)))
                         (if (is-item-active i)
                           (if (<= c 100)
                             (if (= (remainder c 5) 0)
                               (let* ((rotation (- (get-item-rotation i) 1.0))
                                      (x (+ (get-item-x i) 16 (* 64 (cos rotation))))
                                      (y (+ (get-item-y i) (* 64 (sin rotation)))))
                                 (spawn-projectile (make-projectile "sword" x y (+ (get-item-rotation i) 1.57079) i))
                                 (set-item-charge i (+ (get-item-charge i) 1))))
                             (begin
                               (deactivate-item i)
                               (set-item-charge i 0)))))))
