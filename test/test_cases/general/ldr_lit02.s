st: 
ldr x0, L1
ldr x1, L2

b end

L1: 
    .int 0xf
    .int 0xa
L2: 
    .int 0xb

end:


and x0, x0, x0
