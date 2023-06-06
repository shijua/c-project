#include "../inputformat.h"
#include "../Util.h"
struct DPI_instruction
{
    long long * rd;
    bool sf;
    int opc;
    long long operand;
    bool opi;
    int topBit;
};

struct arithmetic_Operand{
    bool sh;
    long long * rn;
    int imm12;

};

struct wideMove_Operand{
    int hw;
    int imm16;
};

extern void DPI(char* memory, struct Registers* registers, struct send_DPI divide);
extern void arithmetic (struct Registers* registers, struct DPI_instruction instr , struct arithmetic_Operand opr);
extern void wideMove (struct Registers* registers, struct DPI_instruction instr, struct wideMove_Operand opr);
