#include <stdio.h>
#include "Util.h"
// Function to get the value of specific bits
unsigned int get_bit(int startBit, int numBits, unsigned int number) {
    // Create a bitmask with the desired number of bits
    unsigned int bitmask = (1 << numBits) - 1;
    // Shift the result back to the rightmost bits
    unsigned int result = number >> (startBit - numBits + 1);
    // Apply the bitmask to the number using bitwise AND operation
    result &= bitmask;
    return result;
}

// Function to get the value of specific bits
unsigned long long get_bitl(int startBit, int numBits, unsigned long long number) {
    // Create a bitmask with the desired number of bits
    unsigned long long bitmask = (1 << numBits) - 1;
    // Shift the result back to the rightmost bits
    unsigned long long result = number >> (startBit - numBits + 1);
    // Apply the bitmask to the number using bitwise AND operation
    result &= bitmask;
    return result;
}

unsigned int getBitRange(int num, int x, int y) {
    unsigned int mask = (1u << (y - x + 1)) - 1;
    return (num >> x) & mask;
}

bool hasCarryOut(long long a, long long b, bool is_64) {
    // if it is 32 then shifting for still detecting overflow as a, b is in long long
    if (!is_64) {
        a <<= 32;
        b <<= 32;
    }
    if ((a < 0 || b < 0)) return 0;
    return a + b < a || a + b < b;
}

bool hasBorrow(long long a, long long b, bool is_64) {
    if (!is_64) {
        a <<= 32;
        b <<= 32;
    };
    if (a < 0) return hasCarryOut(-a, b, is_64);
    return a - b > a;
}

// function for overflow and underflow
bool overflow(long long a, long long b, bool is_64)
{
    // if it is 32 then shifting for still detecting overflow as a, b is in long long
    if (!is_64) { 
        a <<= 32;
        b <<= 32;
    }
    if ((a <= 0 && b >= 0 ) || (a >= 0 && b <= 0)) return false;
    if(a < 0) return (a + b > a);
    return (a + b < a);
}
// int main(int argc, char const *argv[])
// {
//     printf("%d\n", get_bit(28, 4, 335544322));
//     return 0;
// }
