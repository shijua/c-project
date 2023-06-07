#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "input.h"
int readfile(char *buffer, char *filename) {
    FILE* input = fopen(filename, "r");  // Open the file in read 
    assert(input != NULL);
    // Get the file size
    fseek(input, 0, SEEK_END); // Go to the end of the file
    int file_size = ftell(input);
    fseek(input, 0, SEEK_SET); // Go back to the beginning of the file
    // Read the file into a buffer
    buffer = malloc(file_size + 1);
    fread(buffer, file_size, 1, input);
    // close file after store into buffer
    fclose(input);
    return file_size;
}