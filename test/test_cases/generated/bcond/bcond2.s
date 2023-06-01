
cmp x29, x29
add x17, x29, #2279, lsl #12
cmp x17, x29

b.ge L2
add x26, x26, #0x925, lsl #12
L2:
add x26, x26, #0x9a, lsl #0
and x0, x0, x0

