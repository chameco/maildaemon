(define ACID_BLOB (build-entity-prototype "acid_blob" 32 32 10 4 40))

(set-entity-init ACID_BLOB (lambda (e) 0))

(set-entity-update ACID_BLOB (generate-update-track-player 3 5))
