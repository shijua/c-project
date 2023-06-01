ldr x2,D0
add x1,x2,#2
cmp  x3,#2
b.ne L0
add x1,x2,#5
L0:
    and x0,x0,x0
D0:
  .int 0x20200020
  .int 0
.int 0

