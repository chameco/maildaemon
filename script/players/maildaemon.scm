(set-player-stat "move_speed" 6)
(set-player-stat "diag_speed" 5)
(set-player-stat "width" (get-tile-dim))
(set-player-stat "height" (/ (get-tile-dim) 2))
(set-player-stat "level" 0)
(set-player-stat "exp" 0)
(set-player-stat "exp_to_next" 100)
(set-player-stat "health" 100)
(set-player-stat "max_health" 100)
(set-player-stat "regen" 0.1)

(set-player-item 1 (make-item "laser"))
(set-player-item 2 (make-item "flamethrower"))

(set-player-item-index 1)