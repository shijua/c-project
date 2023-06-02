#include "nop.h"
void nop(int* memory, struct Registers *r){
    // Do nothing      
    r->PC += 4;
    return;
}
