#include "sdt.h"
#include <string.h>
long long getRegisterValue(int n, struct Registers* regs) {
    /*if (n <= 30) {
        return regs->general[n];
    } else {
        return regs->SP;
    }*/
    return n <= 30 ? regs -> general[n] : regs -> SP;
}
int sget_bit(int startBit, int numBits, int number) {
    // Create a bitmask with the desired number of bits
    unsigned int bitmask = (1 << numBits) - 1;
    // Shift the result back to the rightmost bits
    int result = number >> (startBit - numBits + 1);
    // Apply the bitmask to the number using bitwise AND operation
    result &= bitmask;
    if ((result & 256) != 0) result -= 512;
    return result;
}
void SingleDataTransfer(char* memory, struct Registers* regs, struct sdtp s) {
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
            int simm9 = sget_bit(10, 9, offset);
            if (bit11 == 1) {
                //Pre-Index
                address = getRegisterValue(n, regs) + ((long long) simm9);
                if (n <= 30) {
                    regs->general[n] = address;
                } else {
                    regs->SP = address;
                }
            } else {
                //Post-Index
                address = getRegisterValue(n, regs);
                if (n <= 30) {
                    regs->general[n] = address + ((long long) simm9);
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

    // start process
    if (sf == 0) {
        // target reg is 32-bits
        if (L == 1) {
            //load
            if (rt <= 30) {
                memcpy(&(regs->general[rt]), memory+address, 4);
            } else {
                memcpy(&(regs->SP), memory+address, 4);
            }
        } else {
            //store
            if (rt <= 30) {
                memcpy(memory+address, &(regs->general[rt])+4, 4);
            } else {
                memcpy(memory+address, &(regs->SP)+4, 4);
            }
        }
    } else {
        // target reg is 64-bits
        if (L == 1) {
            //load
            long long content;
            memcpy(&content, memory+address, 8);
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
            memcpy(memory+address, &value, 8);
        }
    }
}
