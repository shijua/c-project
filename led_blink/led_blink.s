main: 
    ldr w5, zero 
    ldr w11, wait_time 
    ldr w10, one 
wait_temp: 
    adds w5, w5, #1 
    cmp w5, w11
    b.lt wait_temp
    nop
    b turn_led 
buffer_all: 
    .int 32 
    .int 0 
    .int 0x00038041 
    .int 8 
    .int 0 
    .int 130 
    .int 1 
    .int 0 
turn_led:
    ldr w9, start_address 
    ldr w0, write_address 
    ldr w1, led_data 
    str w10, [w9, #0x18] 
check_write_status:
    ldr w12, status_address
    ldr w8, [w12, #0x0]
    and w8, w8, msb_checker
    cmp w8, #0
    b.ne check_write_status
write:
    str w1, [w0] 
reload_buffer:
    ldr w14, request_size 
    str w14, [w9, #0x0]
    ldr w14, zero
    str w14, [w9, #0x4]
    ldr w14, tag_id
    str w14, [w9, #0x8]
    ldr w14, buffer_size
    str w14, [w9, #0xc]
    ldr w14, zero
    str w14, [w9, #0x10]
    ldr w14, pin_num
    str w14, [w9, #0x14]
    ldr w14, zero
    str w14, [w9, #0x1c]
check_read_status:
    ldr w12, status_address
    ldr w8, [w12, #0x0]
    and w8, w8, second_msb_checker
    cmp w8, #0
    b.ne check_read_status
read:
    ldr w13, read_address 
    ldr w2, [w13, #0x0] 
    ldr w5, zero 
flip:
    ldr w15, one
    eor w10, w10, w15  
    b wait_temp
start_address:
    .int 0x80020 
one: 
    .int 1
zero:
    .int 0
led_data:
    .int 0x80028 
read_address:
    .int 0x3f00b880
write_address:
    .int 0x3f00b8a0
status_address:
    .int 0x3f00b8b8
wait_time:
    .int 1000000
msb_checker:
    .int 0x80000000
second_msb_checker:
    .int 0x40000000  
request_size:
    .int 32
tag_id:
    .int 0x00038041
buffer_size:
    .int 8
pin_num:
    .int 130
