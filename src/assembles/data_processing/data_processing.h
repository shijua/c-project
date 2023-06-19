#include <stdint.h>
#include "../parseformat.h"

extern void tokenise_add_sub_immediate(uint32_t *instr, struct add_sub_immediate divide);

extern void tokenise_add_sub_register(uint32_t *instr, struct add_sub_register divide);

extern void tokenise_logical(uint32_t *instr, struct logical divide);

extern void tokenise_move_wide(uint32_t *instr, struct move_wide divide);

void tokenise_multiply(uint32_t *instr, struct multiply divide);

void copy_arithm_opc(uint32_t *instr, char *operend);
