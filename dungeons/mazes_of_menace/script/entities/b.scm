(define ACID_BLOB (build-entity-prototype "acid_blob" 32 32 10 4 40))
(set-entity-init ACID_BLOB (generate-init-standard))
(set-entity-collide ACID_BLOB (generate-collide-damage 5))
(set-entity-update ACID_BLOB (generate-update-track-player 3 5))

(define QUIVERING_BLOB (build-entity-prototype "quivering_blob" 32 32 20 4 40))
(set-entity-init QUIVERING_BLOB (generate-init-standard))
(set-entity-collide QUIVERING_BLOB (generate-collide-damage 10))
(set-entity-update QUIVERING_BLOB (generate-update-track-player 3 5))

(define GELATINOUS_CUBE (build-entity-prototype "gelatinous_cube" 32 32 30 4 40))
(set-entity-init GELATINOUS_CUBE (generate-init-standard))
(set-entity-collide GELATINOUS_CUBE (generate-collide-damage 15))
(set-entity-update GELATINOUS_CUBE (generate-update-track-player 2 8))
