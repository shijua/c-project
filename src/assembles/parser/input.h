#include <stdio.h>
#include "../symboltable.h"
extern int readfile(char *buffer, char *filename);
extern void build_symbol_table(char *buffer, struct symbol_table *table, int file_size);
extern void generate_binary(char *buffer, char *filename, struct symbol_table *table, int file_size);