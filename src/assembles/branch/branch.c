#include <stdlib.h>
#include "branch.h"
#include "string.h"
#include "stdio.h"
#include "assert.h"

static void set_condition_label(uint32_t *instruction, char *label, struct symbol_table *table, u_int8_t cond, u_int32_t address) {
    copy_bit(instruction, cond, 0, 3); // cond to bit 0 to 3
    copy_bit(instruction, 0, 4, 4);
    copy_bit(instruction, 0, 24, 25);
    copy_bit(instruction, (symbol_table_get(table, label) - address) / 4, 5, 23); //sim19
}

static void set_condition_num(uint32_t *instruction, int num, struct symbol_table *table, u_int8_t cond, u_int32_t address) {
    copy_bit(instruction, cond, 0, 3); // cond to bit 0 to 3
    copy_bit(instruction, 0, 4, 4);
    copy_bit(instruction, 0, 24, 25);
    copy_bit(instruction, (num - address) / 4, 5, 23); //sim19
}

static void set_condition(uint32_t *instruction, struct branch divide, struct symbol_table *table, u_int8_t cond, u_int32_t address) {
    if (is_label(divide.literal)) {
        set_condition_label(instruction, divide.literal, table, cond, address);
    } else {
        set_condition_num(instruction, to_int_without_hash(divide.literal), table, cond, address);
    }
}

void tokenise_branch(uint32_t *instruction, struct branch divide, struct symbol_table *table, u_int32_t address) {
    char *op = divide.opcode; // the pointer of operand of the branch instruction
    u_int8_t sf = 1; //condition case
    int32_t operand;
    if (strcmp(op, "b") == 0) {
        // PC = literal
        sf = 0;
        if (is_label(divide.literal)) {
            operand = (symbol_table_get(table, divide.literal) - address) / 4;
        } else {
            operand = (to_int_without_hash(divide.literal) - address) / 4;
        }
        copy_bit(instruction, operand, 0, 25);  //sim26

    } else if (strcmp(op, "br") == 0) {
        // PC = Xn
        sf = 3;
        copy_bit(instruction, 0, 0, 4);
        copy_bit(instruction, 0x87C0, 10, 25);
        copy_bit(instruction, register_to_bin(divide.literal), 5, 9); //Xn
    } else if (strcmp(op, "eq") == 0) {
        // equal
        set_condition(instruction, divide, table, 0x0, address);
    } else if (strcmp(op, "ne") == 0) {
        // not equal
        set_condition(instruction, divide, table, 0x1, address);
    } else if (strcmp(op, "ge") == 0) {
        // signed greater than or equal
        set_condition(instruction, divide, table, 0xa, address);
    } else if (strcmp(op, "lt") == 0) {
        // signed less than
        set_condition(instruction, divide, table, 0xb, address);
    } else if (strcmp(op, "gt") == 0) {
        // signed greater than
        set_condition(instruction, divide, table, 0xc, address);
    } else if (strcmp(op, "le") == 0) {
        // signed less than or equal
        set_condition(instruction, divide, table, 0xd, address);
    } else if (strcmp(op, "al") == 0) {
        // always
        set_condition(instruction, divide, table, 0xe, address);
    } else {
        printf("Error: invalid instruction\n");
        assert(false);
    }
    copy_bit(instruction, sf, 30, 31); // sf to bit 30 and 31
    copy_bit(instruction, 0x5, 26, 29); // 0101 to bit 26 to 29
}
