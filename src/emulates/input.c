#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Main.c"
#include "inputformat.c"
struct Registers registers;
extern void DPI(int* memory, struct Registers registers, struct send_DPI divide){return;};
extern void DPR(int* memory, struct Registers registers, struct send_DPR divide){return;};
extern void SDT(int* memory, struct Registers registers, struct send_SDT divide){return;};
extern void LL(int* memory, struct Registers registers, struct send_LL divide){return;};
extern void branch(int* memory, struct Registers registers, struct send_branch divide){return;};

void to_binary(char in, char out[]) {
    for (int i = 0; i < 8; i++) {
        int bit = (in >> i) & 1;
        out[i] = bit + '0';
    }
}

void hex_to_bin(char in[], char out[]) {
    for (int i = 0; i < 4; i++) {
        char c[8];
        to_binary(in[i], c);
        memcpy(out + i * 8, c, 8);
    }
}

void decode(char next_bin[]) {
    char op0[4];
    strcpy(op0, next_bin+3);
    if (strcmp(op0, "1000") || strcmp(op0, "1001")) {
        DPI(memory, registers, to_DPI(next_bin));
    }else if (strcmp(op0, "0101")) {
        DPR(memory, registers, to_DPR(next_bin));
    }else if (strcmp(op0, "1100") && *next_bin+4 == '1') {
        SDT(memory, registers, to_SDT(next_bin));
    }else if (strcmp(op0, "1101") && *next_bin+4 == '0') {
        LL(memory, registers, to_LL(next_bin));
    }else if (strcmp(op0, "1010") || strcmp(op0, "1011")) {
        branch(memory, registers, to_branch(next_bin));
    } else {
        printf("Error: invalid instruction\n");
        exit(1);
    }
}

void get_instruction(char* buffer, int file_size, int* memory) {
    // getting binary format of each instruction
    int const BYTE_PER_INSTRCUTION = 4;
    for (int i = 0; i < file_size / BYTE_PER_INSTRCUTION; i++) {
        char next_hex[BYTE_PER_INSTRCUTION+1] = "";
        char next_bin[33] = "";  
        memcpy(next_hex, buffer + i * BYTE_PER_INSTRCUTION, BYTE_PER_INSTRCUTION);
        hex_to_bin(next_hex, next_bin);
        printf("%s\n", next_bin);
        memcpy(memory+i, next_bin, 32);
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
    // TODO decode by using PC


    // decode(next_bin);
    return 0;
}
