#include "nop.h"
void nop(int* memory, struct Registers *r){
    r->PC += 4;
}
