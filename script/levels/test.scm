(define (spawn-torch x y)
  (spawn-lightsource (make-lightsource (+ x 16) (+ y 16) 200 3 (make-color 0.7 0.7 0.5 1))) (spawn-fx (make-fx 1 (make-color 0.1 0.1 0.1 1) (+ x 16) y 8 50 100)))
  
(spawn-torch 64 0)
(spawn-torch 192 0)
(spawn-torch 320 0)
(spawn-torch 448 0)
(spawn-torch 576 0)

(spawn-torch 320 320)

(spawn-entity (make-entity "slime" 256 256))
(spawn-entity (make-entity "slime" 256 288))
(spawn-entity (make-entity "human_civilian" 128 128))

(spawn-entity (make-stairs "other" 160 320))

(warp-player 32 32)

;(make-item "turret")
