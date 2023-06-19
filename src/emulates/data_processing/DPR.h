#include <stdbool.h>
#include "../inputformat.h"
#include "../Util.h"
struct DPR_instruction
{
    int64_t * rd;
    int64_t * rn;
    int64_t * rm;
    bool sf;
    int32_t opc;
    int64_t operand;
    int32_t opr;
    bool M;
    int32_t topBit;
};

extern void DPR(char* memory, struct Registers* registers, struct send_DPR divide);
extern void Logical_Operation(struct DPR_instruction instr , int64_t OP2 , struct Registers* registers); 
extern void Arithmetic_Operation (struct DPR_instruction instr , int64_t OP2, struct Registers* registers);
