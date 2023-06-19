w_start:
    mov x0, #0x3F00B880
    mov x1, #0x3F00B8D0
    mov x2, #0x80000
    mov x3, #0x00038041
    mov x4, #130

blink_loop:
    mov w5, #0x1
    b mailbox_write

    mov w5, #0x0
    b mailbox_write

    b blink_loop

mailbox_write:
    mov x8, #0x80000000

wait_mailbox_not_full:
    ldr w6, [x1]
    tst w6, x8
    b.ne wait_mailbox_not_full

    mov x6, #0x800C0

    str x3, [x6, #8]
    str x4, [x6, #20]
    str x5, [x6, #22]

    orr x6, x6, #8
    str x6, [x0, #0x20]

    b wait

wait:
    mov x7, #0xFFFF
wait_loop:
    sub x7, x7, #1
    cmp x7, #0
    b.ne wait_loop

    b blink_loop
