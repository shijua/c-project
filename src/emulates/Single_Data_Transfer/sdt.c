#include "sdt.h"
long long getRegisterValue(int n, struct Registers* regs) {
    /*if (n <= 30) {
        return regs->general[n];
    } else {
        return regs->SP;
    }*/
    return n <= 30 ? regs -> general[n] : regs -> SP;
}
void SingleDataTransfer(int* memory, struct Registers* regs, struct sdtp s) {
    unsigned long long address;
    int offset = s.offset;
    int n = s.xn;
    int rt = s.rt;
    bool sf = s.sf;
    bool L = s.L;
    bool U = s.U;
    if (U == 1) {
        // Unsigned Immediate Offset
        address = getRegisterValue(n, regs) + (unsigned long long) offset;
    } else {
        unsigned int bit21 = get_bit(11, 1, offset);
        if (bit21 == 0) {
            //Pre/Post-Index
            unsigned int bit11 = get_bit(1, 1, offset);
            int simm9 = get_bit(10, 9, offset);
            if (bit11 == 1) {
                //Pre-Index
                address = getRegisterValue(n, regs) + ((long long) simm9);
                if (rt <= 30) {
                    regs->general[rt] = address;
                } else {
                    regs->SP = address;
                }
            } else {
                //Post-Index
                address = getRegisterValue(n, regs);
                if (rt <= 30) {
                    regs->general[rt] = address + ((long long) simm9);
                } else {
                    regs->SP = address + ((long long) simm9);
                }
            }
        } else {
            // Register Offset
            unsigned int m = get_bit(10, 5, offset);
            long long xm = getRegisterValue((int) m, regs);
            long long xn = getRegisterValue(n, regs);
            address = xm + xn;
        }
    }
    address /= 4;

    // start process
    if (sf == 0) {
        // target reg is 32-bits
        if (L == 1) {
            //load
            if (rt <= 30) {
                regs->general[rt] = memory[address];
            } else {
                regs->SP = memory[address];
            }
        } else {
            //store
            if (rt <= 30) {
                memory[address] = regs->general[rt];
            } else {
                memory[address] = regs->SP;
            }
        }
    } else {
        // target reg is 64-bits
        if (L == 1) {
            //load
            int intValue1 = memory[address];
            int intValue2 = memory[address + 1];
            long long content = ((long long)intValue2 & 0xFFFFFFFF) + (((long long)intValue1 & 0xFFFFFFFF) << 32);
            if (rt <= 30) {
                regs->general[rt] = content;
            } else {
                regs->SP = content;
            }
        } else {
            //store
            long long value;
            if (rt <= 30) {
                value = regs->general[rt];
            } else {
                value = regs->SP;
            }
            int intvalue1 = (int) value & 0x00000000FFFFFFFF;
            int intvalue2 = (int) ((value & 0xFFFFFFFF00000000) >> 32);
            //long long content = ((long long)intvalue1 & 0xFFFFFFFF) + (((long long)intvalue2 & 0xFFFFFFFF) << 32);
            memory[address] = intvalue2;
            memory[address + 1] = intvalue1;
        }
    }
}
