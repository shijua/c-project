#include "bitwiseShift.h"
#include <stdbool.h>


int64_t logicalShiftLeft(int64_t value, int shiftAmount, bool is64Bit) {
    if (is64Bit) {
        return value << shiftAmount;  //shift left with int64_t (64-bit)
    } else {
        return (int32_t)(value) << shiftAmount; //shift left with int(32-bit)
    }
}

int64_t logicalShiftRight(int64_t value, int shiftAmount, bool is64Bit) {
    if (is64Bit) {
        return (uint64_t)(value) >> shiftAmount; //shift right with int64_t (64-bit)
    } else {
        return (uint32_t)(value) >> shiftAmount; //shift right with int (32-bit)
    }
}


int64_t arithmeticShiftRight(int64_t value, int shiftAmount, bool is64Bit) {
    if (is64Bit) {
        return value >> shiftAmount;//shift right with int64_t (64-bit) but not unsigned. Making it arithmetic shift right
    } else {
        return (int32_t)(value) >> shiftAmount;
    }
}


int64_t rotateRight(int64_t value, int shiftAmount, bool is64Bit) { 
    if (is64Bit) {
        const int numBits = 64;
        shiftAmount %= numBits;// Ensure shiftAmount is within the range of valid bits
        int64_t left = value << (numBits - shiftAmount);
        int64_t right = logicalShiftRight(value, shiftAmount, is64Bit); // Left shift value by (numBits - shiftAmount) bits
        int64_t result = right | left;
        return result;
        // Right shift value by shiftAmount bits and left shift value by (numBits - shiftAmount) bits
        // Then perform bitwise OR between the shifted values to get the final result

    } else {
        const int numBits = 32;
        shiftAmount %= numBits;// Ensure shiftAmount is within the range of valid bits
        return ((uint32_t)(value) >> shiftAmount) | ((uint32_t)(value) << (numBits - shiftAmount));
        // Cast value to uint32_t to discard sign extension
        // Right shift value by shiftAmount bits and left shift value by (numBits - shiftAmount) bits . Then perform bitwise OR between the shifted values to get the final result
    }
}
