#include <stdio.h>
#include <stdbool.h>
#include "../memory.h"
#include "output.h"

void output_general(long long* general_register_ptr) {
    for (int i = 0; i < 31; i++) {
        printf("X%d=%016llX\n", i, *(general_register_ptr+i));
    }
}

void output_Pstate(bool* pstate_ptr) {
    for (int i = 0; i < 4; i++) {
        if (*(pstate_ptr+i) == false) {
            printf("-");
        } else {
            switch (i){
                case 0: printf("N"); break;
                case 1: printf("Z"); break;
                case 2: printf("C"); break;
                case 3: printf("V"); break;
            }
        }
    }
}

void output_memory(long long* memory){
    for (int i = 0; i < 524288; i++) // 2^19
    {
        if (memory[i] != 0) {
            printf("0x%08X: 0x%08X\n", i*4, memory[i]);
        }
    }
}


void output(struct Registers* register_ptr, long long* memory) { 
    // output all general registers
    printf("Registers:\n");
    output_general(register_ptr->general);

    // output PC
    printf("PC: %016llX\n", register_ptr->PC);

    // output P-state
    printf("PSTATE :");
    output_Pstate(&(register_ptr->pstate.N));
    printf("\n");

    //output memory
    printf("Non-zero memory:\n");
    output_memory(memory);
}

// int main(int argc, char const *argv[])
// {
//     struct Registers registers;

//     // initialise 
//     long long* general_register_ptr = registers.general;
//     for (int i = 0; i < 31; i++) {
//         *general_register_ptr++ = 0;
//     }
//     registers.general[0] = 3;
//     registers.general[1] = 1;
//     registers.general[2] = 2;

//     // initialise pstate
//     registers.pstate.N = false;
//     registers.pstate.Z = false;
//     registers.pstate.C = true;
//     registers.pstate.V = false;

//     // initialise memory
//     for (int i = 0; i < 524288; i++) // 2^19
//     {
//         memory[i] = 0;
//     }
//     memory[0] = 114514;
//     memory[114514] = 1919810;

//     output(&registers);

//     return 0;
// }