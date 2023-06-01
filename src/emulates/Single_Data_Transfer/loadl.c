//
// Created by LIHAOMIN on 2023/5/30.
//

#include "loadl.h"
#include <stdbool.h>

void LoadLiteral(int* memory, struct Registers* regs, struct loadliteral l) {
    unsigned long long address;
    bool sf = l.sf;
    long long simm19 = l.simm19;
    int rt = l.rt;
    address = (simm19 + regs->PC) / 4;
    if (sf == 0) {
        // target reg is 32-bits
        if (rt <= 30) {
            regs->general[rt] = memory[address];
        } else {
            regs->SP = memory[address];
        }
    } else {
        // target reg is 64-bits
        int intValue1 = memory[address];
        int intValue2 = memory[address + 1];
        long long content = ((long long)intValue2 & 0xFFFFFFFF) + (((long long)intValue1 & 0xFFFFFFFF) << 32);
        if (rt <= 30) {
            regs->general[rt] = content;
        } else {
            regs->SP = content;
        }
    }
}
