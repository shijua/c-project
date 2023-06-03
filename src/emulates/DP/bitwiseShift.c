#include "bitwiseShift.h"
#include <stdbool.h>


long long logicalShiftLeft(long long value, int shiftAmount, bool is64Bit) {
    if (is64Bit) {
        return value << shiftAmount;  //shift left with long long (64-bit)
    } else {
        return (int)(value) << shiftAmount; //shift left with int(32-bit)
    }
}

long long logicalShiftRight(long long value, int shiftAmount, bool is64Bit) {
    if (is64Bit) {
        return (unsigned long long)(value) >> shiftAmount; //shift right with long long (64-bit)
    } else {
        return (unsigned int)(value) >> shiftAmount; //shift right with int (32-bit)
    }
}


long long arithmeticShiftRight(long long value, int shiftAmount, bool is64Bit) {
    if (is64Bit) {
        return value >> shiftAmount;//shift right with long long (64-bit) but not unsigned. Making it arithmetic shift right
    } else {
        return (int)(value) >> shiftAmount;
    }
}


long long rotateRight(long long value, int shiftAmount, bool is64Bit) { 
    if (is64Bit) {
        const int numBits = 64;
        shiftAmount %= numBits;// Ensure shiftAmount is within the range of valid bits
        return (value >> shiftAmount) | (value << (numBits - shiftAmount)); 
        // Right shift value by shiftAmount bits and left shift value by (numBits - shiftAmount) bits
        // Then perform bitwise OR between the shifted values to get the final result

    } else {
        const int numBits = 32;
        shiftAmount %= numBits;// Ensure shiftAmount is within the range of valid bits
        return ((unsigned int)(value) >> shiftAmount) | ((unsigned int)(value) << (numBits - shiftAmount));
        // Cast value to unsigned int to discard sign extension
        // Right shift value by shiftAmount bits and left shift value by (numBits - shiftAmount) bits . Then perform bitwise OR between the shifted values to get the final result
    }
}
