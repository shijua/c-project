#include "sdt.h"
#include <string.h>
long long getRegisterValue(int n, struct Registers* regs) {
    return n <= 30 ? regs -> general[n] : regs -> SP;
}

void SingleDataTransfer(char* memory, struct Registers* regs, struct sdtp s) {
    // init variables
    unsigned long long address;
    int n = s.xn;
    if (s.U == 1) {
        // Unsigned Immediate Offset
        address = getRegisterValue(n, regs) + (unsigned long long) s.offset * 8;
    } else {
        // check bit 21
        unsigned int bit21 = get_bit(11, 1, s.offset);
        if (bit21 == 0) {
            // Pre/Post-Index
            unsigned int bit11 = get_bit(1, 1, s.offset);
            int simm9 = sget_bit(10, 9, s.offset);
            if (bit11 == 1) {
                // Pre-Index
                address = getRegisterValue(n, regs) + ((long long) simm9);
                if (n <= 30) {
                    regs->general[n] = address;
                } else {
                    regs->SP = address;
                }
            } else {
                // Post-Index
                address = getRegisterValue(n, regs);
                if (n <= 30) {
                    regs->general[n] = address + ((long long) simm9);
                } else {
                    regs->SP = address + ((long long) simm9);
                }
            }
        } else {
            // Register Offset
            unsigned int m = get_bit(10, 5, s.offset);
            long long xm = getRegisterValue((int) m, regs);
            long long xn = getRegisterValue(n, regs);
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
            long long content;
            // get content from memory
            memcpy(&content, memory+address, 8);
            if (s.rt <= 30) {
                regs->general[s.rt] = content;
            } else {
                regs->SP = content;
            }
        } else {
            //store
            long long value;
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
