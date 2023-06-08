#include "branch.h"
#include "string.h"
#include "stdio.h"
#include "assert.h"
/*
void tokenise_branch(unsigned int *instruction, struct branch divide, struct symbol_table *table) {
    char *op = divide.opcode; // the pointer of operand of the branch instruction
    int sf = 1; //condition case
    int operand;
    if (strcmp(op, "b") == 0) {
        // PC = literal
        sf = 0;



    } else if (strcmp(op, "br") == 0) {
        // PC = Xn
        sf = 3;

    } else if (strcmp(op, "eq") == 0) {
        // equal
        // ... code ...

    } else if (strcmp(op, "ne") == 0) {
        // not equal
        // ... code ...

    } else if (strcmp(op, "ge") == 0) {
        // signed greater than or equal
        // ... code ...

    } else if (strcmp(op, "lt") == 0) {
        // signed less than
        // ... code ...

    } else if (strcmp(op, "gt") == 0) {
        // signed greater than
        // ... code ...

    } else if (strcmp(op, "le") == 0) {
        // signed less than or equal
        // ... code ...

    } else if (strcmp(op, "al") == 0) {
        // always
        // ... code ...

    } else {
        printf("Error: invalid instruction\n");
        assert(false);

    }
    copy_bit(instruction, sf, 30, 31); // sf to bit 30 and 31
    copy_bit(instruction,0x5, 26, 29); // 0101 to bit 26 to 29
    copy_bit(instruction, operand, 0, 3); // cond to bit 0 to 3
}
*/