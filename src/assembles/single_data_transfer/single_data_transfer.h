#include "../Util.h"
#include "../parseformat.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../symboltable.h"
extern void tokenise_load_store(unsigned int *instruction, struct load_store divide, struct symbol_table *table);
extern void tokenise_load_store_literal(unsigned int *instruction, struct load_store_literal divide, struct symbol_table *table, int address);
