#include "loadl.h"
#include <stdbool.h>
#include <string.h>

void LoadLiteral(char* memory, struct Registers* regs, struct loadliteral l) {
    // init variables
    unsigned long long address;
    bool sf = l.sf;
    long long simm19 = l.simm19;
    int rt = l.rt;
    address = (simm19 + regs->PC);
    if (sf == 0) {
        // target reg is 32-bits
        if (rt <= 30) {
            memcpy(&(regs->general[rt]), memory+address, 4);
        } else {
            memcpy(&(regs->SP), memory+address, 4);
        }
    } else {
        // target reg is 64-bits
        long long content;
        // get content from memory
        memcpy(&content, memory+address, 8);
        if (rt <= 30) {
            regs->general[rt] = content;
        } else {
            regs->SP = content;
        }
    }
}
