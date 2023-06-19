#include "../inputformat.h"
#include "../Util.h"
struct DPI_instruction
{
    int64_t * rd;
    bool sf;
    uint8_t opc;
    int64_t operand;
    bool opi;
    uint8_t topBit;
};

struct arithmetic_Operand{
    bool sh;
    int64_t * rn;
    int32_t imm12;

};

struct wideMove_Operand{
    uint8_t hw;
    int32_t imm16;
};

extern void DPI(char* memory, struct Registers* registers, struct send_DPI divide);
extern void arithmetic (struct Registers* registers, struct DPI_instruction instr , struct arithmetic_Operand opr);
extern void wideMove (struct Registers* registers, struct DPI_instruction instr, struct wideMove_Operand opr);
