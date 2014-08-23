(define-standard-gui-mode
  "not_found"
  (lambda ()
    (reset-gui)
    (spawn-label (- (/ (get-screen-width) 2) 304) 100 "No saved game found" 4.0)
    (spawn-button
      (- (/ (get-screen-width) 2) 100)
      (- (/ (get-screen-height) 2) 100)
      "return" (lambda ()
                 (schedule
                   (lambda ()
                     (set-mode "main_menu"))
                   0)))))
