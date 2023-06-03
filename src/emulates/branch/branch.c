#include"branch.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void process(struct send_branch pre, struct literal *lit){
    lit->sf = pre.sf;
    lit->simM26 = pre.operand;
    lit->Xn = getBitRange(pre.operand, 5, 9);
    lit->SimM19 = getBitRange(pre.operand, 5, 23);
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
            }
            break;
        default:
            printf("Error: invalid instruction\n");
            exit(1);
    }
}

bool conditional_branch(struct Registers * r, unsigned int condition) {
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
            exit(1);
    }
    return false;
}


// int main() {
//     unsigned int number = 1234567890;  // You can replace this with your actual number
//     int x = 2;
//     int y = 6;
//     int result = getBitRange(number, x, y);
//     printf("The binary number from bit %d to bit %d is: %d\n", x, y, result);
//     return 0;
// }

// void test_Branch() {
//     int memory[10] = {0};
//     struct Registers reg;
//     struct send_branch pre;
//     struct literal lit;

//     // Test case 1: sf = 0
//     pre.sf = 0;
//     pre.operand = 2;
//     reg.PC = 0;
//     Branch(memory, &reg, pre);
//     assert(reg.PC == 8);

//     // Test case 2: sf = 3
//     pre.sf = 3;
//     pre.operand = 32; // Xn = 1
//     reg.general[1] = 5;
//     reg.PC = 0;
    
//     Branch(memory, &reg, pre);
//     assert(reg.PC == 5);

//     // Test case 3: sf = 1, condition met (Z == 1)
//     pre.sf = 1;
//     pre.operand = 0; // cond = 0
//     reg.PC = 0;
//     reg.pstate.Z = 1;
//     Branch(memory, &reg, pre);
//     assert(reg.PC == 0);

    
// }

// int main() {
//     test_Branch();
//     printf("All tests passed!\n");
//     return 0;
// }
