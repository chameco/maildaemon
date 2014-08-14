(lambda ()
  (reset-gui)
  (spawn-label (- (/ (get-screen-width) 2) 576) 100 "game over" 16.0)
  (spawn-button
    (- (/ (get-screen-width) 2) 100)
    (- (/ (get-screen-height) 2) 100)
    "menu" (lambda ()
             (schedule
               (lambda ()
                 (reset-game)
                 (load-game 1)
                 (set-mode "main_menu"))
               0)))
  (spawn-button
    (- (/ (get-screen-width) 2) 100)
    (- (/ (get-screen-height) 2) 25)
    "quit" (lambda () (set-running #f)))
  )
