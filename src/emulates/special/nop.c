#include "nop.h"
void nop(char* memory, struct Registers *r){
    // Do nothing      
    r->PC += 4;
    return;
}
