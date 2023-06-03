#include <stdbool.h>
#include "../inputformat.h"
#include "../Util.h"
struct DPR_instruction
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

void DPR(char* memory, struct Registers* registers, struct send_DPR divide);
void arithAndLogic(struct DPR_instruction instr , long long OP2 , struct Registers* registers); 

