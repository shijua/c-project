#include"branch.h"

unsigned int getBitRange(int num, int x, int y) {
    unsigned int mask = (1u << (y - x + 1)) - 1;
    return (num >> x) & mask;
}

void printBinary(unsigned int num) {
    for (int i = sizeof(num) * 8 - 1; i >= 0; i--) {
        putchar('0' + ((num >> i) & 1));
    }
    putchar('\n');
}

void process(struct send_branch pre, struct literal *lit){
    lit->sf = pre.sf;
    lit->simM26 = pre.operand;
    lit->Xn = getBitRange(pre.operand, 5, 9);
    lit->SimM19 = getBitRange(pre.operand, 5, 23);
    lit->cond = getBitRange(pre.operand, 0, 3);
}
//extern void branch(int* memory, struct Registers registers, struct send_branch divide){return;}
void Branch(int* memory, struct Register *r, struct send_branch pre){
    struct literal *lit
    process(pre, lit);
    switch (lit->sf) {
        case 0:
            r->PC += (lit->simM26)*4;
            break;
        case 3:
            r->PC = r->gen[lit->Xn];
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

bool conditional_branch(struct Register * reg, unsigned int condition) {
    switch (condition) {
        case 0:
            return reg->pstate.Z == 1;
        case 1:
            return reg->pstate.Z == 0;
        case 10:
            return reg->pstate.N == 1;
        case 11:
            return reg->pstate.N == 0;
        case 12:
            return reg->pstate.Z == 0 && reg->pstate.N == reg->pstate.V;
        case 13:
            return !(reg->pstate.Z == 0 && reg->pstate.N == reg->pstate.V);
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