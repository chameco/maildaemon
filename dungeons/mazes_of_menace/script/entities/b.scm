(define ACID_BLOB (build-entity-prototype "acid_blob" 32 32 10 4 40))

(set-entity-init ACID_BLOB (lambda (e) 0))

(set-entity-collide
  ACID_BLOB
  (lambda (e data)
    (if (= data 0)
      (hit-player 5))
    data))

(set-entity-update ACID_BLOB (generate-update-track-player 3 5))
