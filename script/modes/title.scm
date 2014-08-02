(define-mode
  "title"
  (lambda ()
    (spawn-image (- (/ (get-screen-width) 2) 200) (- (/ (get-screen-height) 2) 200) "textures/title.png"))
  (lambda () #f)
  (lambda () #f))
