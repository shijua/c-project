#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "input.h"
#include "../inputformat.h"
#include "../Single_Data_Transfer/sdt.h"
#include "../Single_Data_Transfer/loadl.h"
#include "../branch/branch.h"
#include "../special/halting.h"
#include "../special/nop.h"
#include "../DP/DPI.h"
#include "../DP/DPR.h"
// extern void DPI(char* memory, struct Registers* registers, struct send_DPI divide){return;}
// extern void DPR(char* memory, struct Registers* registers, struct send_DPR divide){return;}
// extern void SingleDataTransfer(char* memory, struct Registers* registers, struct sdtp divide){return;}
// extern void LoadLiteral(char* memory, struct Registers* registers, struct loadliteral divide){return;}
// extern void Branch(char* memory, struct Registers* registers, struct send_branch divide){return;}

void decode(char* memory, struct Registers* registers, unsigned int instruction) {
    int op0 = get_bit(28, 4, instruction);
    // printf("op0: %d\n", op0);
    if (instruction == -721215457) {
        nop(memory, registers);
    } else if (instruction == -1979711488) {
        halting(memory, registers);
    } else if (op0 == 8 || op0 == 9) {  // 1000 1001
        DPI(memory, registers, to_DPI(instruction));
        registers->PC += 4;
    } else if (op0 == 5 || op0 == 13) { // 0101 1101
        DPR(memory, registers, to_DPR(instruction));
        registers->PC += 4;
    } else if (op0 == 12 && get_bit(29, 1, instruction) == 1) { // 1100
        SingleDataTransfer(memory, registers, to_SDT(instruction));
        registers->PC += 4;
    } else if (op0 == 12 && get_bit(29, 1, instruction) == 0) { // 1100
        LoadLiteral(memory, registers, to_LL(instruction));
        registers->PC += 4;
    } else if (op0 == 10 || op0 == 11) { // 1010 1011
        Branch(memory, registers, to_branch(instruction));
    } else {
        printf("Error: invalid instruction\n");
        printf("op0: %d\n", op0);
        assert(false);
    }
}

void get_instruction(char* buffer, int file_size, char* memory) {
    // getting binary format of each instruction
    for (int i = 0; i < file_size; i++) {
        memcpy(memory + i, buffer + i, 1);
    }
}

void readfile(char* filename, char* memory) {
    FILE *file = fopen(filename, "rb");  // Open the binary file in read 
    assert(file != NULL);

    // Get the file size
    fseek(file, 0, SEEK_END); // Go to the end of the file
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET); // Go back to the beginning of the file

    // Read the file into a buffer
    char *buffer = malloc(file_size + 1);
    assert(buffer != NULL);
    fread(buffer, file_size, 1, file);
    buffer[file_size] = '\0';

    get_instruction(buffer, file_size, memory);
    // Close the file
    free(buffer);
    fclose(file);

    // printf("Read file successfully\n");
}

// int main(int argc, char const *argv[]){
//     readfile("b1_exp.bin", memory);
//     decode(memory[registers.PC / 4]);
//     return 0;
// }
