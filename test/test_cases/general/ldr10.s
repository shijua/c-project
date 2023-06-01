L0: 
    ldr x1, [x0], #2
    ldr x2, L0
    ldr x0, D1
    add x0, x0, #2
    b L1
L1: 
    ldr x0, D2
    b end
D1: 
    .int 0x1
D2: 
    .int 0x2

hang:
    b hang

end:
    and x0, x0, x0
