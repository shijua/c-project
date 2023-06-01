#include <bitwiseShift.h>
#include <stdbool.h>


long long logicalShiftLeft(long long value, int shiftAmount, bool is64Bit) {
    if (is64Bit) {
        return value << shiftAmount;
    } else {
        return (int)(value) << shiftAmount;
    }
}

long long logicalShiftRight(long long value, int shiftAmount, bool is64Bit) {
    if (is64Bit) {
        return (unsigned long long)(value) >> shiftAmount;
    } else {
        return (unsigned int)(value) >> shiftAmount;
    }
}


long long arithmeticShiftRight(long long value, int shiftAmount, bool is64Bit) {
    if (is64Bit) {
        return value >> shiftAmount;
    } else {
        return (int)(value) >> shiftAmount;
    }
}


long long rotateRight(long long value, int shiftAmount, bool is64Bit) {
    if (is64Bit) {
        const int numBits = 64;
        shiftAmount %= numBits;
        return (value >> shiftAmount) | (value << (numBits - shiftAmount));
    } else {
        const int numBits = 32;
        shiftAmount %= numBits;
        return ((unsigned int)(value) >> shiftAmount) | ((unsigned int)(value) << (numBits - shiftAmount));
    }
}
