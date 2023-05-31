#include "nop.h"
void nop(int* memory, struct Register *r, struct send_branch pre){
    r->PC += 4;
}
