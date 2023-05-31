#include <stdio.h>
// Function to get the value of specific bits
unsigned int get_bit(int startBit, int numBits, unsigned int number) {
    // Create a bitmask with the desired number of bits
    unsigned int bitmask = (1 << numBits) - 1;
    // Shift the result back to the rightmost bits
    unsigned int result = number >> startBit;
    // Apply the bitmask to the number using bitwise AND operation
    result = number & bitmask;
    return result;
}


// int main(int argc, char const *argv[])
// {
//     get_bit(28, 4, 335544322);
//     return 0;
// }
