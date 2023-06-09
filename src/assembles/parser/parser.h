#include "../parseformat.h"
#include "../symboltable.h"
extern void to_alias(char *opcode, char *remain);
extern void parse_add_sub(unsigned int *instruction, char *opcode);
extern void parse_logical(unsigned int *instruction, char *opcode);
extern void parse_move_wide(unsigned int *instruction, char *opcode);
extern void parse_multiply(unsigned int *instruction, char *opcode);
extern void parse_load_store(unsigned int *instruction, char *opcode, struct symbol_table *table, int address);
extern void parse(char *in, int address, unsigned int *instruction, struct symbol_table *table);
