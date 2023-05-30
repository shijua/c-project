#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../../emulate.h"
#include "inputformat.h"
#include "input.h"
#include "../Util.h"

int memory[MAX_MEMORY];
struct Registers registers;
extern void DPI(int* memory, struct Registers registers, struct send_DPI divide){return;};
extern void DPR(int* memory, struct Registers registers, struct send_DPR divide){return;};
extern void SDT(int* memory, struct Registers registers, struct send_SDT divide){return;};
extern void LL(int* memory, struct Registers registers, struct send_LL divide){return;};
extern void branch(int* memory, struct Registers registers, struct send_branch divide){return;};

void decode(int instruction) {
    int op0 = get_bit(28, 4, instruction);
    if (op0 == 0b1000 || op0 == 0b1001) {
        DPI(memory, registers, to_DPI(instruction));
    }else if (op0 == 0b0101) {
        DPR(memory, registers, to_DPR(instruction));
    }else if (op0 == 0b1100 && get_bit(24, 1, instruction) == 1) {
        SDT(memory, registers, to_SDT(instruction));
    }else if (op0 == 0b1101 && get_bit(24, 1, instruction) == 0) {
        LL(memory, registers, to_LL(instruction));
    }else if (op0 == 0b1010 || op0 == 0b1011) {
        branch(memory, registers, to_branch(instruction));
    } else {
        printf("Error: invalid instruction\n");
        exit(1);
    }
}

void get_instruction(char* buffer, int file_size, int* memory) {
    // getting binary format of each instruction
    int const BYTE_PER_INSTRCUTION = 4;
    for (int i = 0; i < file_size / BYTE_PER_INSTRCUTION; i++) {
        int instruction = 0;
        memcpy(memory + i, buffer + i * BYTE_PER_INSTRCUTION, BYTE_PER_INSTRCUTION);
    }
}

void readfile(char* filename, int* memory) {
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
}

int main(int argc, char const *argv[]){
    readfile("b1_exp.bin", memory);
    decode(memory[registers.PC / 4]);
    return 0;
}
