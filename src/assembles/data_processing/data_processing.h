#include "parseformat.h"

extern void tokenise_add_sub_immediate (unsigned int* instr , struct add_sub_immediate divide);

extern void tokenise_add_sub_register (unsigned int* instr , struct add_sub_register divide);

extern void tokenise_logical (unsigned int* instr , struct logical divide);

extern void tokenise_move_wide (unsigned int* instr , struct move_wide divide);

void tokenise_multiply(unsigned int *instr, struct multiply divide);

void copy_arithm_opc (unsigned int *instr , char* operend);
