(build-item-prototype "turret"
                      (lambda (i data)
                        (begin
                          (spawn-projectile (make-color 0 0 1 1)
                                            0 160 
                                            16 0
                                            8 8
                                            100
                                            i
                                            20)
                          (deactivate-item i))))
