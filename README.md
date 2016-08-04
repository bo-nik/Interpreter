## Code example
```
program
var i1, j1, max_i, max_j, i, j : float
begin
    read(i1, max_i, j1, max_j)
    do i = i1 to max_i
         do j = j1 to max_j
            if not [i == j] then goto m1
            write(i, j)
            m1:
        next
    next
end
```
