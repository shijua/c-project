
add x6, x29, #317, lsl #12
cmp x6, x29

b.gt L4
add x12, x12, #0xcaf, lsl #0
L4:
add x12, x12, #1999, lsl #0
and x0, x0, x0

