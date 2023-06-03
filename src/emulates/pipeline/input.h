#include "../Util.h"
extern void decode(char* memory, struct Registers* registers, unsigned int instruction);
extern void get_instruction(char* buffer, int file_size, char* memory);
extern void readfile(char* filename, char* memory);
