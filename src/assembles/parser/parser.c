#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "parser.h"
#include "../Util.h"

// will be comment later after include other header file for tokenise
void tokenise_add_sub_immediate(unsigned int *instruction, struct add_sub_immediate divide) { return; }
void tokenise_add_sub_register(unsigned int *instruction, struct add_sub_register divide) { return; }
void tokenise_logical(unsigned int *instruction, struct logical divide) { return; }
void tokenise_move_wide(unsigned int *instruction, struct move_wide divide) { return; }
void tokenise_multiply(unsigned int *instruction, struct multiply divide) { return; }
void tokenise_branch(unsigned int *instruction, struct branch divide, struct symbol_table *table) { return; }
void tokenise_load_store(unsigned int *instruction, struct load_store divide, struct symbol_table *table) { return; }
void tokenise_load_store_shift(unsigned int *instruction, struct load_store_shift divide, struct symbol_table *table) { return; }
void tokenise_load_store_literal(unsigned int *instruction, struct load_store_literal divide, struct symbol_table *table) { return; }
void tokenise_int(unsigned int *instruction, struct constant divide) { return; }

// void concat(char *str1, char *str2) {
//     char *result = malloc(strlen(str1) + strlen(str2) + 1);
//     strcpy(result, str1);
//     strcat(result, str2);
//     return result;
// }


// TODO: implement this function
// returns the alias of the opcode and remain part of the string
void to_alias(char *opcode, char *remain)
{
    return;
    // if (!strcmp(opcode, "cmp"))
    // {
    //     opcode = "sub";

    // }
    // else if (!strcmp(opcode, "cmn"))
    // {
    //     /* code */
    // }
    // else if (!strcmp(opcode, "neg"))
    // {
    //     /* code */
    // }
    // else if (!strcmp(opcode, "negs"))
    // {
    //     /* code */
    // }
    // else if (!strcmp(opcode, "tst"))
    // {
    //     /* code */
    // }
    // else if (!strcmp(opcode, "mvn"))
    // {
    //     /* code */
    // }

    // else if (!strcmp(opcode, "mov"))
    // {
    //     /* code */
    // }

    // else if (!strcmp(opcode, "mul"))
    // {
    //     /* code */
    // }
    // else if (!strcmp(opcode, "mneg"))
    // {
    //     /* code */
    // }
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

void parse_load_store(unsigned int *instruction, char *opcode, struct symbol_table *table)
{
    char *rt = strtok(NULL, ",");
    char *xn = strtok(NULL, ",");
    char *simm = strtok(NULL, ",");
    if (simm == NULL)
    {
        // then only two parameters (only for ldr)
        assert(strcmp(opcode, "ldr") == 0);
        tokenise_load_store_literal(instruction, load_store_literal_init(opcode, rt), table);
    }
    else if (simm[0] == '#')
    {
        // then it is post or pre or unsigned offset
        tokenise_load_store(instruction, load_store_init(opcode, rt, xn, simm), table);
    }
    else
    {
        // then it is register offset
        char *shift = strtok(NULL, ",");
        tokenise_load_store_shift(instruction, load_store_shift_init(opcode, rt, xn, simm, shift), table);
    }
}

// overall parsing for each line
void parse(char *in, int address, unsigned int *instruction, struct symbol_table *table)
{
    // printf("Parsing: %s\n", in);
    char *opcode = strtok(in, " ");
    // if operend is analias shown in spec, then change it to the real one
    if (!strcmp(opcode, "cmp") || !strcmp(opcode, "cmn") || !strcmp(opcode, "neg") ||
        !strcmp(opcode, "negs") || !strcmp(opcode, "tst") || !strcmp(opcode, "mvn") ||
        !strcmp(opcode, "mov") || !strcmp(opcode, "mul") || !strcmp(opcode, "meng"))
    {
        to_alias(opcode, in);
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
        parse_load_store(instruction, opcode, table);
    }
    // special
    else if (!strcmp(opcode, "nop"))
    {
        *instruction = 0xd503201f;
    }
    else if (!strcmp(opcode, ".int"))
    {
        char *constant = strtok(NULL, ","); // literal or xn
        tokenise_int(instruction, constant_init(opcode, constant));
    }
    else
    {
        printf("Error: unknown instruction\n");
    }
}
