#include <stdio.h>
#include <stdbool.h>
#include "../Util.h"
#include "output.h"

void output_general(long long* general_register_ptr, FILE* fp) {
    for (int i = 0; i < 31; i++) {
        // output 16 hexadecimals
        fprintf(fp, "X%02d    = %016llx\n", i, *(general_register_ptr+i));
    }
}

void output_Pstate(bool* pstate_ptr, FILE* fp) {
    for (int i = 0; i < 4; i++) {
        // if flag is 0
        if (*(pstate_ptr+i) == false) {
            fprintf(fp, "-");
        } else {
            switch (i){
                case 0: fprintf(fp,"N"); break;
                case 1: fprintf(fp,"Z"); break;
                case 2: fprintf(fp,"C"); break;
                case 3: fprintf(fp,"V"); break;
            }
        }
    }
}

void output_memory(int* memory, FILE* fp){
    // output all memory that exists
    for (int i = 0; i < 524288; i++) // 2^19
    {
        if (memory[i] != 0) {
            fprintf(fp, "0x%08x : %08x\n", i*4, memory[i]);
        }
    }
}


void output(struct Registers* register_ptr, int* memory, FILE* fp) { 
    // output all general registers
    fprintf(fp, "Registers:\n");
    output_general(register_ptr->general, fp);

    // output PC
    fprintf(fp, "PC     = %016llX\n", register_ptr->PC);

    // output P-state
    fprintf(fp, "PSTATE : ");
    output_Pstate(&(register_ptr->pstate.N), fp);
    fprintf(fp, "\n");

    //output memory
    fprintf(fp, "Non-Zero Memory:\n");
    output_memory(memory, fp);
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
