#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "parser.h"
#include "../Util.h"
#include "../single_data_transfer/single_data_transfer.h"
#include "../data_processing/data_processing.h"
#include "../branch/branch.h"
#include "../special/special.h"
// will be comment later after include other header file for tokenise
// void tokenise_add_sub_immediate(unsigned int *instruction, struct add_sub_immediate divide) { return; }
// void tokenise_add_sub_register(unsigned int *instruction, struct add_sub_register divide) { return; }
// void tokenise_logical(unsigned int *instruction, struct logical divide) { return; }
// void tokenise_move_wide(unsigned int *instruction, struct move_wide divide) { return; }
// void tokenise_multiply(unsigned int *instruction, struct multiply divide) { return; }
// void tokenise_branch(unsigned int *instruction, struct branch divide, struct symbol_table *table) { return; }
// void tokenise_load_store(unsigned int *instruction, struct load_store divide, struct symbol_table *table) { return; }
// void tokenise_load_store_literal(unsigned int *instruction, struct load_store_literal divide, struct symbol_table *table, int address) { return; }
//void tokenise_int(unsigned int *instruction, struct constant divide) { return; }

// concat str2 to str1
// pre: new will be given to the variable of str1
static void concat(char *str1, char *str2)
{
    // let temp be the pointer of str1
    str1 = realloc(str1, strlen(str1) + strlen(str2) + 1);
    strcat(str1, str2);
}

// insert rzr to the left of the string
static void insert_left(char *new, char *first, char *second)
{
    concat(new, "rzr,");
    concat(new, first);
    concat(new, ",");
    concat(new, second);
}

// insert rzr to the middle of the string
static void insert_middle(char *new, char *first, char *second)
{
    concat(new, first);
    concat(new, ",rzr,");
    concat(new, second);
}

// insert rzr to the right of the string
static void insert_right(char *new, char *first, char *second, char *third)
{
    concat(new, first);
    concat(new, ",");
    concat(new, second);
    concat(new, ",");
    concat(new, third);
    concat(new, ",rzr");
}

// returns the alias of original line
void to_alias(char *opcode, char *remain)
{
    // get remaining token from instruction
    char *first = strtok(NULL, ",");
    char *second = strtok(NULL, ",");
    char *third = strtok(NULL, ",");
    // use to free at the end
    char *new = malloc(6);
    if (!strcmp(opcode, "cmp"))
    {
        // subs rzr,rn,<op2>
        strcpy(new, "subs ");
        insert_left(new, first, second);
    }
    else if (!strcmp(opcode, "cmn"))
    {
        // adds rzr,rn,<op2>
        strcpy(new, "adds ");
        insert_left(new, first, second);
    }
    else if (!strcmp(opcode, "neg"))
    {
        // sub rd,rzr,<op2>
        strcpy(new, "sub ");
        insert_middle(new, first, second);
    }
    else if (!strcmp(opcode, "negs"))
    {
        // subs rd,rzr,<op2>
        strcpy(new, "subs ");
        insert_middle(new, first, second);
    }
    else if (!strcmp(opcode, "tst"))
    {
        // ands rzr,rn,<op2>
        strcpy(new, "ands ");
        insert_left(new, first, second);
    }
    else if (!strcmp(opcode, "mvn"))
    {
        // orn rd,rzr,<op2>
        strcpy(new, "orn ");
        insert_middle(new, first, second);
    }

    else if (!strcmp(opcode, "mov"))
    {
        // orr rd,rzr,<op2>
        strcpy(new, "orr ");
        insert_middle(new, first, second);
    }

    else if (!strcmp(opcode, "mul"))
    {
        // madd rd,rn,rm,rzr
        strcpy(new, "madd ");
        insert_right(new, first, second, third);
    }
    else if (!strcmp(opcode, "mneg"))
    {
        // msub rd,rn,rm,rzr
        strcpy(new, "msub ");
        insert_right(new, first, second, third);
    }
    remain = realloc(remain, strlen(new) + 1);
    strcpy(remain, new);
    free(new);
}

// parsing each specific operation
void parse_add_sub(unsigned int *instruction, char *opcode)
{
    char *rd = strtok(NULL, ",");
    char *rn = strtok(NULL, ",");
    char *imm = strtok(NULL, ",");
    char *shift = strtok(NULL, ",");
    if (imm[0] == '#')
    {
        // then immediate instruction
        tokenise_add_sub_immediate(instruction, add_sub_immdiate_init(opcode, rd, rn, imm, shift));
    }
    else
    {
        // then register instruction
        tokenise_add_sub_register(instruction, add_sub_register_init(opcode, rd, rn, imm, shift));
    }
}

void parse_logical(unsigned int *instruction, char *opcode)
{
    char *rd = strtok(NULL, ",");
    char *rn = strtok(NULL, ",");
    char *rm = strtok(NULL, ",");
    char *shift = strtok(NULL, ",");
    tokenise_logical(instruction, logical_init(opcode, rd, rn, rm, shift));
}

void parse_move_wide(unsigned int *instruction, char *opcode)
{
    char *rd = strtok(NULL, ",");
    char *imm = strtok(NULL, ",");
    char *shift = strtok(NULL, ",");
    tokenise_move_wide(instruction, move_wide_init(opcode, rd, imm, shift));
}

void parse_multiply(unsigned int *instruction, char *opcode)
{
    char *rd = strtok(NULL, ",");
    char *rn = strtok(NULL, ",");
    char *rm = strtok(NULL, ",");
    char *ra = strtok(NULL, ",");
    tokenise_multiply(instruction, multiply_init(opcode, rd, rn, rm, ra));
}

void parse_load_store(unsigned int *instruction, char *opcode, struct symbol_table *table, int address)
{
    char *rt = strtok(NULL, ",");
    char *xn = strtok(NULL, ",");
    char *simm = strtok(NULL, ",");
    if (simm == NULL)
    {
        // then only two parameters (only for ldr)
        assert(!strcmp(opcode, "ldr"));
        tokenise_load_store_literal(instruction, load_store_literal_init(opcode, rt, xn), table, address);
    }
    else
    {
        // then it is post or pre or unsigned offset
        tokenise_load_store(instruction, load_store_init(opcode, rt, xn, simm), table);
    }
}

// overall parsing for each line
void parse(char *in, int address, unsigned int *instruction, struct symbol_table *table)
{
    // printf("Parsing: %s\n", in);
    char *opcode = strtok(in, " ");
    // if opcode is analias shown in spec, then change it to the real one
    if (!strcmp(opcode, "cmp") || !strcmp(opcode, "cmn") || !strcmp(opcode, "neg") ||
        !strcmp(opcode, "negs") || !strcmp(opcode, "tst") || !strcmp(opcode, "mvn") ||
        !strcmp(opcode, "mov") || !strcmp(opcode, "mul") || !strcmp(opcode, "meng"))
    {
        to_alias(opcode, in);
        opcode = strtok(in, " ");
    }
    // data processing
    if (!strcmp(opcode, "add") || !strcmp(opcode, "adds") ||
        !strcmp(opcode, "sub") || !strcmp(opcode, "subs"))
    {
        parse_add_sub(instruction, opcode);
    }
    else if (!strcmp(opcode, "and") || !strcmp(opcode, "ands") ||
             !strcmp(opcode, "bic") || !strcmp(opcode, "bics") ||
             !strcmp(opcode, "eor") || !strcmp(opcode, "orr") ||
             !strcmp(opcode, "eon") || !strcmp(opcode, "orn"))
    {
        parse_logical(instruction, opcode);
    }
    else if (!strcmp(opcode, "movk") || !strcmp(opcode, "movn") || !strcmp(opcode, "movz"))
    {
        parse_move_wide(instruction, opcode);
    }
    else if (!strcmp(opcode, "madd") || !strcmp(opcode, "msub"))
    {
        parse_multiply(instruction, opcode);
    }
    // branch
    else if (!strcmp(opcode, "b") || !strcmp(opcode, "br"))
    {
        char *literal = strtok(NULL, ","); // literal or xn
        tokenise_branch(instruction, branch_init(opcode, literal), table);
    }
    // brach with condition
    else if (!strcmp(opcode, "b.eq") || !strcmp(opcode, "b.ne") || !strcmp(opcode, "b.ge") ||
             !strcmp(opcode, "b.lt") || !strcmp(opcode, "b.gt") || !strcmp(opcode, "b.le") ||
             !strcmp(opcode, "b.al"))
    {
        char *literal = strtok(NULL, ","); // literal or xn
        // only send eq ne .etc if it is branch condition
        char *cond = strtok(opcode, ".");
        cond = strtok(NULL, ".");
        tokenise_branch(instruction, branch_init(cond, literal), table);
    }
    // loads and stores
    else if (!strcmp(opcode, "ldr") || !strcmp(opcode, "str"))
    {
        parse_load_store(instruction, opcode, table, address);
    }
    // special
    else if (!strcmp(opcode, "nop"))
    {
        *instruction = 0xd503201f;
    }
    else if (!strcmp(opcode, ".int"))
    {
        char *constant = strtok(NULL, " "); // literal or xn
        tokenise_int(instruction, constant_init(opcode, constant));
    }
    else
    {
        printf("Error: unknown instruction\n");
    }
}
