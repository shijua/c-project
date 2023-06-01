#include <input.h>

struct instruction
{
    long long * rd;
    long long * rn;
    long long * rm;
    bool sf;
    int opc;
    long long operand;
    int opr;
    bool M;
};

void DPR(int* memory, struct Registers* registers, struct send_DPR divide);
void arithAndLogic(struct instruction instr , long long OP2 , struct Registers* registers); 