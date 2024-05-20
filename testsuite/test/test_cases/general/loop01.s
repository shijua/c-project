ldr x2,D0
wait:
sub x2,x2,#1
cmp x2,#0x0
b.ne wait

and x0, x0, x0
D0:
.int 0x11
.int 0


