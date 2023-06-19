#include "sdt.h"
#include <string.h>
int64_t getRegisterValue(int n, struct Registers* regs) {
    return n <= 30 ? regs -> general[n] : regs -> SP;
}

void SingleDataTransfer(char* memory, struct Registers* regs, struct sdtp s) {
    // init variables
    uint64_t address;
    uint8_t n = s.xn;
    if (s.U == 1) {
        // Unsigned Immediate Offset
        address = getRegisterValue(n, regs) + (uint64_t) s.offset * 8;
    } else {
        // check bit 21
        uint32_t bit21 = get_bit(11, 1, s.offset);
        if (bit21 == 0) {
            // Pre/Post-Index
            uint32_t bit11 = get_bit(1, 1, s.offset);
            int simm9 = sget_bit(10, 9, s.offset);
            if (bit11 == 1) {
                // Pre-Index
                address = getRegisterValue(n, regs) + ((int64_t) simm9);
                if (n <= 30) {
                    regs->general[n] = address;
                } else {
                    regs->SP = address;
                }
            } else {
                // Post-Index
                address = getRegisterValue(n, regs);
                if (n <= 30) {
                    regs->general[n] = address + ((int64_t) simm9);
                } else {
                    regs->SP = address + ((int64_t) simm9);
                }
            }
        } else {
            // Register Offset
            uint32_t m = get_bit(10, 5, s.offset);
            int64_t xm = getRegisterValue((int) m, regs);
            int64_t xn = getRegisterValue(n, regs);
            address = xm + xn;
        }
    }

    // start process
    if (s.sf == 0) {
        // target reg is 32-bits
        if (s.L == 1) {
            // load
            if (s.rt <= 30) {
                memcpy(&(regs->general[s.rt]), memory+address, 4);
            } else {
                memcpy(&(regs->SP), memory+address, 4);
            }
        } else {
            // store
            if (s.rt <= 30) {
                memcpy(memory+address, &(regs->general[s.rt]), 4);
            } else {
                memcpy(memory+address, &(regs->SP), 4);
            }
        }
    } else {
        // target reg is 64-bits
        if (s.L == 1) {
            //load
            int64_t content;
            // get content from memory
            memcpy(&content, memory+address, 8);
            if (s.rt <= 30) {
                regs->general[s.rt] = content;
            } else {
                regs->SP = content;
            }
        } else {
            //store
            int64_t value;
            // get content from register
            if (s.rt <= 30) {
                value = regs->general[s.rt];
            } else {
                value = regs->SP;
            }
            memcpy(memory+address, &value, 8);
        }
    }
}
