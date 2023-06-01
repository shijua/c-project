st:
ldr x0, Value

b end

Value:
    .int 0xace0f


end:

and x0, x0, x0
