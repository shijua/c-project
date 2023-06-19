#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "output.h"

void output_general(int64_t* general_register_ptr, FILE* fp) {
    for (int i = 0; i < 31; i++) {
        // output 16 hexadecimals
        fprintf(fp, "X%02d    = %016lx\n", i, *(general_register_ptr+i));
    }
}

void output_Pstate(bool* pstate_ptr, FILE* fp) {
    for (int i = 0; i < 4; i++) {
        // if flag is 0
        if (*(pstate_ptr+i) == false) {
            fprintf(fp, "-");
        } else {
            switch (i) {
                case 0: fprintf(fp,"N"); break;
                case 1: fprintf(fp,"Z"); break;
                case 2: fprintf(fp,"C"); break;
                case 3: fprintf(fp,"V"); break;
            }
        }
    }
}

void output_memory(char* memory, FILE* fp){
    // output all memory that exists
    for (int i = 0; i < 524288; i++) // 2^19
    {
        int instruction;
        memcpy(&instruction, memory+i*4, 4);
        if (instruction != 0) {
            fprintf(fp, "0x%08x : %08x\n", i*4, instruction);
        }
    }
}


void output(struct Registers* register_ptr, char* memory, FILE* fp) { 
    // output all general registers
    fprintf(fp, "Registers:\n");
    output_general(register_ptr->general, fp);

    // output PC
    fprintf(fp, "PC     = %016lx\n", register_ptr->PC);

    // output P-state
    fprintf(fp, "PSTATE : ");
    output_Pstate(&(register_ptr->pstate.N), fp);
    fprintf(fp, "\n");

    //output memory
    fprintf(fp, "Non-Zero Memory:\n");
    output_memory(memory, fp);
}
