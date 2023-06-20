main: 
    ldr w5, zero ;set w5 to 0 , w5 is the counter for loop
    ldr w11, wait_time ;w11 is the wait time
    ldr w10, one ; w10 is the led status, it's set to 1 by defult
wait_temp: ;wait for a while
    adds w5, w5, #1 
    cmp w5, w11
    b.lt wait_temp
    nop ;place holder for buffer to be 16-bit aligned
    b turn_led ;jump to turn led
buffer_all: 
    .int 32 ;request size
    .int 0 ;request code
    .int 0x00038041 ;tag id
    .int 8 ;value buffer size
    .int 0 ;request
    .int 130 ;power led pin number
    .int 1 ;led status
    .int 0 ;end tag
turn_led:
    ldr w9, start_address ;the buffer start address
    ldr w0, write_address ;the address of write register
    ldr w1, led_data ;the content of led_data(0x80028)
    str w10, [w9, #0x18] ;write to led status
check_write_status:
    ldr w12, status_address
    ldr w8, [w12, #0x0]
    and w8, w8, msb_checker
    cmp w8, #0 ;if F is true then keep checking
    b.ne check_write_status
write:
    str w1, [w0] ;update write register
reload_buffer:
    ldr w14, request_size ;reload all the buffer
    str w14, [w9, #0x0]
    ldr w14, zero ;request code
    str w14, [w9, #0x4]
    ldr w14, tag_id ;tag id
    str w14, [w9, #0x8]
    ldr w14, buffer_size ;value buffer size
    str w14, [w9, #0xc]
    ldr w14, zero ;request code
    str w14, [w9, #0x10]
    ldr w14, pin_num ;power led pin number
    str w14, [w9, #0x14]
    ldr w14, zero ;end tag
    str w14, [w9, #0x1c]
check_read_status:
    ldr w12, status_address
    ldr w8, [w12, #0x0]
    and w8, w8, second_msb_checker
    cmp w8, #0 ;if E is true then keep checking
    b.ne check_read_status
read:
    ldr w13, read_address ;load the address of read register
    ldr w2, [w13, #0x0] ;read
    ldr w5, zero ;set w5 to 0 , w5 is the counter for loop
flip:
    ldr w15, one
    eor w10, w10, w15  ;filp the bits of led status
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