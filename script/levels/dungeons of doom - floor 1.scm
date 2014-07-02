(spawn-torch 64 0)
(spawn-torch 192 0)
(spawn-torch 320 0)
(spawn-torch 448 0)
(spawn-torch 576 0)

(spawn-torch 320 320)

(let ((mover (make-entity "grid_bug" 256 256)))
 (move-entity mover 0)
 (spawn-entity mover))
(spawn-entity (make-entity "acid_blob" 256 288))
(spawn-entity (make-entity "human_civilian" 128 128))

(set-current-dialog "You are alone")
