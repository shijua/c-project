#include <stdio.h>
#include <stdbool.h>
int const MAX_MEMORY = 2100000; // 2^21  2097152
long long memory[MAX_MEMORY];

struct PSTATE
{
    bool N, Z, C, V;
};

struct Registers {
    // int R0 to R30 for 31 genreal purpose registers
    long long R0, R1, R2, R3, R4, R5, R6, R7, R8, R9,
        R10, R11, R12, R13, R14, R15, R16, R17, R18, R19,
        R20, R21, R22, R23, R24, R25, R26, R27, R28, R29,
        R30;
    // Special registers
    long long PC, ZR, SP;
    // processor state register
    struct PSTATE pstate;
};

int main(int argc, char *argv[]) {
    // struct Registers registers;
    // registers.pstate.N = 0;
    // printf("%d", registers.pstate.N);
    return 0;
}
