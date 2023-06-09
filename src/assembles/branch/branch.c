#include <stdlib.h>
#include "branch.h"
#include "string.h"
#include "stdio.h"
#include "assert.h"
bool is_label(char *literal){
    if (literal[0] == '#') {
        return false;
    }
    return true;
}
void set_condition_label(unsigned int *instruction, char *label, struct symbol_table *table, int cond) {
    copy_bit(instruction, cond, 0, 3); // cond to bit 0 to 3
    copy_bit(instruction, 0, 4, 4);
    copy_bit(instruction, 0, 24, 25);
    copy_bit(instruction, symbol_table_get(table, label), 5, 23); //sim19
}
void set_condition_num(unsigned int *instruction, int num, struct symbol_table *table, int cond){
    copy_bit(instruction, cond, 0, 3); // cond to bit 0 to 3
    copy_bit(instruction, 0, 4, 4);
    copy_bit(instruction, 0, 24, 25);
    copy_bit(instruction, num, 5, 23); //sim19
}
void set_condition(unsigned int *instruction, struct branch divide, struct symbol_table *table, int cond){
    if (is_label(divide.literal)) {
        set_condition_label(instruction, divide.literal, table, cond);
    } else {
        set_condition_num(instruction, to_num(divide.literal), table, cond);
    }
}

void tokenise_branch(unsigned int *instruction, struct branch divide, struct symbol_table *table) {
    char *op = divide.opcode; // the pointer of operand of the branch instruction
    int sf = 1; //condition case
    int operand;
    if (strcmp(op, "b") == 0) {
        // PC = literal
        sf = 0;
        if(is_label(divide.literal)){
            operand = symbol_table_get(table, divide.literal);
        }else{
            operand = to_num(divide.literal);
        }
        copy_bit(instruction, operand, 0, 25); //sim26

    } else if (strcmp(op, "br") == 0) {
        // PC = Xn
        sf = 3;
        copy_bit(instruction, 0, 0, 4);
        copy_bit(instruction, 0x87C0, 5, 25);

    } else if (strcmp(op, "eq") == 0) {
        // equal
        set_condition(instruction, divide, table, 0x0);

    } else if (strcmp(op, "ne") == 0) {
        // not equal
        set_condition(instruction, divide, table, 0x1);
    } else if (strcmp(op, "ge") == 0) {
        // signed greater than or equal
        set_condition(instruction, divide, table, 0xa);
    } else if (strcmp(op, "lt") == 0) {
        // signed less than
        set_condition(instruction, divide, table, 0xb);

    } else if (strcmp(op, "gt") == 0) {
        // signed greater than
        set_condition(instruction, divide, table, 0xc);

    } else if (strcmp(op, "le") == 0) {
        // signed less than or equal
        set_condition(instruction, divide, table, 0xd);

    } else if (strcmp(op, "al") == 0) {
        // always
        set_condition(instruction, divide, table, 0xe);

    } else {
        printf("Error: invalid instruction\n");
        assert(false);

    }
    copy_bit(instruction, sf, 30, 31); // sf to bit 30 and 31
    copy_bit(instruction,0x5, 26, 29); // 0101 to bit 26 to 29
}
