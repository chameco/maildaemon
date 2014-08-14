(let xloop ([xl (iota 20)])
 (if (not (null? xl))
   (begin
     (let yloop ([yl (iota 20)])
      (if (not (null? yl))
        (begin
          (set-tile (car xl) (car yl) 3)
          (yloop (cdr yl)))))
     (xloop (cdr xl)))))
