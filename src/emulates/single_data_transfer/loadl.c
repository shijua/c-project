#include "loadl.h"
#include <string.h>

void LoadLiteral(char* memory, struct Registers* regs, struct loadliteral l) {
    // init variables
    uint64_t address;
    address = (l.simm19 * 4 + regs->PC);
    if (l.sf == 0) {
        // target reg is 32-bits
        if (l.rt <= 30) {
            memcpy(&(regs->general[l.rt]), memory+address, 4);
        } else {
            memcpy(&(regs->SP), memory+address, 4);
        }
    } else {
        // target reg is 64-bits
        int64_t content;
        // get content from memory
        memcpy(&content, memory+address, 8);
        if (l.rt <= 30) {
            regs->general[l.rt] = content;
        } else {
            regs->SP = content;
        }
    }
}
