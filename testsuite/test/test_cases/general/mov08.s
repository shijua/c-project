movz x2, #0xBEEF
movk x2, #0xDEAD, lsl #16
movk x2, #0xCAFE, lsl #32

and x0, x0, x0
