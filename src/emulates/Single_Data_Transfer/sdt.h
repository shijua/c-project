#include "../Util.h"
#include "../inputformat.h"
extern int sget_bit(int startBit, int numBits, int number);
extern long long getRegisterValue(int n, struct Registers* regs);
extern void SingleDataTransfer(char* memory, struct Registers* regs, struct sdtp s);
