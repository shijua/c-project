#include "../parseformat.h"
#include "../symboltable.h"
extern void to_alias(char *operand, char *remain);
extern void parse_add_sub(int *instruction, char *operand, char *in);
extern void parse_logical(int *instruction, char *operand, char *in);
extern void parse_move_wide(int *instruction, char *operand, char *in);
extern void parse_multiply(int *instruction, char *operand, char *in);
extern void parse_load_store(int *instruction, char *operand, char *in, struct symbol_table *table);
extern void parse(char *in, int address, int *instruction, struct symbol_table *table);
