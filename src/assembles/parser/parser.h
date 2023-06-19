#include "../parseformat.h"
#include "../symboltable.h"

extern char *to_alias(char *opcode, char *remain);

extern void parse_add_sub(uint32_t *instruction, char *opcode);

extern void parse_logical(uint32_t *instruction, char *opcode);

extern void parse_move_wide(uint32_t *instruction, char *opcode);

extern void parse_multiply(uint32_t *instruction, char *opcode);

extern void parse_load_store(uint32_t *instruction, char *opcode, struct symbol_table *table, uint32_t address);

extern void parse(char *in, uint32_t address, uint32_t *instruction, struct symbol_table *table);
