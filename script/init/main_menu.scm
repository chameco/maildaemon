(define-standard-gui-mode
  "main_menu"
  (lambda ()
    (reset-gui)
    (spawn-label (- (/ (get-screen-width) 2) 640) 100 "maildaemon" 16.0)
    (spawn-image (- (/ (get-screen-width) 2) 500)
                 (- (/ (get-screen-height) 2) 100)
                 "textures/ampersand.png")
    (spawn-image (+ (/ (get-screen-width) 2) 180)
                 (- (/ (get-screen-height) 2) 100)
                 "textures/ampersand.png")
    (spawn-button
      (- (/ (get-screen-width) 2) 100)
      (- (/ (get-screen-height) 2) 100)
      "continue" (lambda ()
                   (if (load-game 1)
                     (schedule
                       (lambda ()
                         (set-mode "main"))
                       0)
                     (schedule
                       (lambda ()
                         (set-mode "not_found"))
                       0))))
    (spawn-button
      (- (/ (get-screen-width) 2) 100)
      (- (/ (get-screen-height) 2) 25)
      "new game" (lambda ()
                   (schedule
                     (lambda ()
                       (set-mode "new_game"))
                     0)))
    (spawn-button
      (- (/ (get-screen-width) 2) 100)
      (+ (/ (get-screen-height) 2) 50)
      "quit" (lambda () (set-running #f)))))
