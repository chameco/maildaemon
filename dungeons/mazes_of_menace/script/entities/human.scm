(define HUMAN-HP 100)
(define HUMAN-SPEED 6)
(define HUMAN-EXP-VALUE 10)

(define HUMAN-CIVILIAN (build-entity-prototype "human_civilian" 32 32 HUMAN-HP HUMAN-SPEED HUMAN-EXP-VALUE))
(set-entity-collide HUMAN-CIVILIAN
                    (lambda (e data)
                      (set-current-dialog "Watch it!")))
(define HUMAN-GUARD (build-entity-prototype "human_guard" 32 32 HUMAN-HP HUMAN-SPEED HUMAN-EXP-VALUE))
(define HUMAN-ARCHER (build-entity-prototype "human_archer" 32 32 HUMAN-HP HUMAN-SPEED HUMAN-EXP-VALUE))
(define HUMAN-MAGE (build-entity-prototype "human_mage" 32 32 HUMAN-HP HUMAN-SPEED HUMAN-EXP-VALUE))
