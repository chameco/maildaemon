(spawn-lightsource (make-lightsource 80 16 256 3 (make-color 0.5 0.5 0 1)))
(spawn-entity (make-entity "slime" 256 256))
(spawn-entity (make-entity "slime" 256 288))
(spawn-entity (make-entity "human_civilian" 128 128))

(warp-player 32 32)

(make-item "turret")
