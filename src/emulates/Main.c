#include <stdio.h>
#include <stdbool.h>
int const MAX_MEMORY = 2100000; // 2^21  2097152
int memory[MAX_MEMORY];

struct PSTATE
{
    bool N, Z, C, V;
};

struct Registers {
    // int R0 to R30 for 31 genreal purpose registers
    long long general[31];
    // Special registers
    long long PC, ZR, SP;
    // processor state register
    struct PSTATE pstate;
};

// int main(int argc, char *argv[]) {
//     struct Registers registers;
//     struct Registers* registers_ptr = &registers;
//     registers_ptr->R1 = 0;
//     // registers.pstate.N = 0;
//     // printf("%d", registers.pstate.N);
//     return 0;
// }
