(define-standard-gui-mode
  "new_game"
  (lambda ()
    (reset-gui)
    (spawn-label (- (/ (get-screen-width) 2) 512) 100 "new game" 16.0)
    (spawn-label (- (/ (get-screen-width) 2) 500) 200 "choose a dungeon to explore" 4.0)
    (spawn-button
      (- (/ (get-screen-width) 2) 100)
      (- (/ (get-screen-height) 2) 100)
      "mazes of menace" (lambda ()
                          (set-dungeon "mazes_of_menace")
                          (switch-level "village field")
                          (warp-player 64 64)
                          (schedule
                            (lambda ()
                              (set-mode "main"))
                            0)))
    (spawn-button
      (- (/ (get-screen-width) 2) 100)
      (- (/ (get-screen-height) 2) 25)
      "temple" (lambda ()
                          (set-dungeon "temple")
                          (switch-level "entryway")
                          (warp-player 64 64)
                          (schedule
                            (lambda ()
                              (set-mode "main"))
                            0)))))
