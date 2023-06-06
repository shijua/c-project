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

// get bit that will return signed int
int sget_bit(int startBit, int numBits, int number) {
    // Create a bitmask with the desired number of bits
    unsigned int bitmask = (1 << numBits) - 1;
    // Shift the result back to the rightmost bits
    int result = number >> (startBit - numBits + 1);
    // Apply the bitmask to the number using bitwise AND operation
    result &= bitmask;
    if ((result & (1 << (numBits - 1))) != 0) result -= 1 << numBits;
    return result;
}

unsigned int getBitRange(int num, int x, int y) {
    unsigned int mask = (1u << (y - x + 1)) - 1;
    return (num >> x) & mask;
}

// function to check whether it has borrow if sub is 1 or carry if sub is 0
bool hasCarryBorrow(long long a, long long b, bool is_64, bool is_sub) {
    if (!is_64) {
        a <<= 32;
        b <<= 32;
    }
    bool carry = 0;
    // if it is subtraction
    if (is_sub) {
        carry = 1;
        b = ~b;
    }
    long long next = 1;
    int count = 0;
    // compare the bit in a and b (emulate full adder to get borrow/carry)
    while (count < 64) {
        bool bita = (a & next) >> count;
        bool bitb = (b & next) >> count;
        carry = (bita + bitb) + carry > 1;
        next <<= 1;
        count++;
    }
    return carry;
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
