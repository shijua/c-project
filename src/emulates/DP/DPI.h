#include "../inputformat.h"
#include "../Util.h"
struct DPI_instruction
{
    long long * rd;
    bool sf;
    int opc;
    long long operand;
    bool opi;
};

struct arithmetic_Operand{
    bool sh;
    long long * rn;
    int imm12;
    long long max_value;
    long long min_value;

};

struct wideMove_Operand{
    int hw;
    int imm16;
};

void DPI(char* memory, struct Registers* registers, struct send_DPI divide);
void arithmetic (struct Registers* registers, struct DPI_instruction instr , struct arithmetic_Operand opr);
void wideMove (struct Registers* registers, struct DPI_instruction instr, struct wideMove_Operand opr);
void intToReg (int reg); 

void copyBits(long long source, long long* destination, int startBit, int endBit);


