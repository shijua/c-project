#include <stdlib.h>
#include"Main.c"
void nop(struct Registers *registers) {
    registers->PC += 4;
}
void halt(struct Registers *registers) {
    exit(0);
}