#include"halting.h"
void halting(int* memory, struct Registers* r){
    // return -5 as PC + 4 after execute, exit when PC = -1
    r->PC = -5;
}
