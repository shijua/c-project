#include <stdint.h>
#include "../Util.h"
#include "../parseformat.h"
#include "../symboltable.h"

extern void tokenise_branch(uint32_t *instruction, struct branch divide, struct symbol_table *table, uint32_t address);
