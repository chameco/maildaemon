(let xloop ([xl (iota 20)])
 (if (not (null? xl))
   (begin
     (let yloop ([yl (iota 20)])
      (if (not (null? yl))
        (begin
          (if (or (= (car xl) 0) (= (car xl) 19))
            (set-tile (car xl) (car yl) 9)
            (if (or (= (car yl) 0) (= (car yl) 19))
              (set-tile (car xl) (car yl) 8)
              (set-tile (car xl) (car yl) 1)))
          (yloop (cdr yl)))))
     (xloop (cdr xl)))))
