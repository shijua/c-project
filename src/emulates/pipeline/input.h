#include "../memory.h"
extern void decode(int* memory, struct Registers registers, int instruction);
extern void get_instruction(char* buffer, int file_size, int* memory);
extern void readfile(char* filename, int* memory);
