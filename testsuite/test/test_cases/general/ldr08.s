ldr x0,D0
ldr x2,D1
add x2,x2,#8

and x0,x0,x0

D0:
  .int 0x20200020
  .int 0
.int 0
D1:
  .int 0x20200028
  .int 0

