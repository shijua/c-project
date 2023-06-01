#include "nop.h"
void nop(int* memory, struct Register *r){
    r->PC += 4;
}
