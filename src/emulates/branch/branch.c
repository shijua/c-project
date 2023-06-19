#include"branch.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void process(struct send_branch pre, struct literal *lit){
    lit->sf = pre.sf;
    lit->simM26 = pre.operand;
    lit->Xn = getBitRange(pre.operand, 5, 9);
    lit->SimM19 = sget_bit(23, 19, pre.operand);
    lit->cond = getBitRange(pre.operand, 0, 3);
}
//extern void branch(char* memory, struct Registers registers, struct send_branch divide){return;}
void Branch(char* memory, struct Registers *r, struct send_branch pre){
    struct literal *lit = malloc(sizeof(struct literal));
    process(pre, lit);
    switch (lit->sf) {
        case 0:
            r->PC += (lit->simM26)*4;
            break;
        case 3:
            r->PC = r->general[lit->Xn];
            break;
        case 1:
            if (conditional_branch(r, lit->cond)) {
                r->PC += (lit->SimM19)*4;
            } else {
                r->PC += 4;
            }
            break;
        default:
            printf("Error: invalid instruction\n");
            assert(false);
    }
    free(lit);
}

bool conditional_branch(struct Registers * r, uint32_t condition) {
    switch (condition) {
        case 0:
            return r->pstate.Z == 1;
        case 1:
            return r->pstate.Z == 0;
        case 10:
            return r->pstate.N == r->pstate.V; //fix the condition by ed
        case 11:
            return r->pstate.N != r->pstate.V; //fix the condition by ed
        case 12:
            return r->pstate.Z == 0 && r->pstate.N == r->pstate.V;
        case 13:
            return !(r->pstate.Z == 0 && r->pstate.N == r->pstate.V);
        case 14:
            return true;
        default:
            printf("Error: invalid instruction\n");
            assert(false);
    }
    return false;
}
