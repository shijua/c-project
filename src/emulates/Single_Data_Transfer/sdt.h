//
// Created by LIHAOMIN on 2023/5/30.
//
#include "../../emulate.h"

struct sdtp { // single data transfer parameter
    bool sf;
    bool L;
    bool U;
    //char offset[12];
    int offset;
    int xn;
    int rt;
};
unsigned int getBits(unsigned int number, int startBit, int numBits);
long long getRegisterValue(int n, struct Registers regs);
void SingleDataTransfer(int* memory, struct Registers regs, struct sdtp s);