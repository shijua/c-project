#include "branch.h"
#include "string.h"
#include "stdio.h"
#include "assert.h"
void set_condition(unsigned int *instruction, char* literal, int cond, struct symbol_table *table){
    copy_bit(instruction, symbol_table_get(table,literal), 5, 23);// sim19 to bit 5 to 23
    copy_bit(instruction, cond, 0, 3); // cond to bit 0 to 3
    copy_bit(instruction, 0, 4,4); // 0 to bit 4
    copy_bit(instruction, 0, 24, 25); // 00 to bit 24 and 25
}

void tokenise_branch(unsigned int *instruction, struct branch divide, struct symbol_table *table) {
    char *op = divide.opcode; // the pointer of operand of the branch instruction
    int cond;
    int sf = 1; //condition case
    if (strcmp(op, "b") == 0) {
        // PC = literal
        sf = 0;
        copy_bit(instruction, symbol_table_get(table, divide.literal),0,25) ; // get the value of the operand
    } else if (strcmp(op, "br") == 0) {
        // PC = Xn
        sf = 3;
        copy_bit(instruction, register_to_bin(divide.literal), 5, 9); // register to bit 5 to 9
        copy_bit(instruction,0x0, 0, 4); // 00000 to bit 0 to 4
        copy_bit(instruction,0x87C0, 10, 25);//1 0 0 0 0 1 1 1 1 1 0 0 0 0 0 0 to bit 10 to 25

    } else if (strcmp(op, "eq") == 0) {
        // equal
        set_condition(instruction,divide.literal, 0, table);
    } else if (strcmp(op, "ne") == 0) {
        // not equal
        set_condition(instruction,divide.literal, 1, table);

    } else if (strcmp(op, "ge") == 0) {
        // signed greater than or equal
        set_condition(instruction,divide.literal, 10, table);

    } else if (strcmp(op, "lt") == 0) {
        // signed less than
        set_condition(instruction,divide.literal, 11, table);

    } else if (strcmp(op, "gt") == 0) {
        // signed greater than
        set_condition(instruction,divide.literal, 12, table);

    } else if (strcmp(op, "le") == 0) {
        // signed less than or equal
        set_condition(instruction,divide.literal, 13, table);

    } else if (strcmp(op, "al") == 0) {
        // always
        set_condition(instruction,divide.literal, 14, table);

    } else {
        printf("Error: invalid instruction\n");
        assert(false);

    }
    copy_bit(instruction, sf, 30, 31); // sf to bit 30 and 31
    copy_bit(instruction,0x5, 26, 29); // 0101 to bit 26 to 29

}
