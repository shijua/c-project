#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "parser.h"
#include "../Util.h"

// will be comment later after include other header file for tokenise
void tokenise_add_sub_immediate(unsigned int *instruction, struct add_sub_immediate divide){return;}
void tokenise_add_sub_register(unsigned int *instruction, struct add_sub_register divide){return;}
void tokenise_logical(unsigned int *instruction, struct logical divide){return;}
void tokenise_move_wide(unsigned int *instruction, struct move_wide divide){return;}
void tokenise_multiply(unsigned int *instruction, struct multiply divide){return;}
void tokenise_branch(unsigned int *instruction, struct branch divide, struct symbol_table *table){return;}
void tokenise_load_store(unsigned int *instruction, struct load_store divide, struct symbol_table *table){return;}
void tokenise_load_store_shift(unsigned int *instruction, struct load_store_shift divide, struct symbol_table *table){return;}
void tokenise_load_store_literal(unsigned int *instruction, struct load_store_literal divide, struct symbol_table *table){return;}
void tokenise_int(unsigned int *instruction, struct constant divide){return;}

// TODO: implement this function
void to_alias(char *operand, char *remain) {return;}
// parsing each specific operation
void parse_add_sub(unsigned int *instruction, char *operand)
{
    char *rd = strtok(NULL, ",");
    char *rn = strtok(NULL, ",");
    char *imm = strtok(NULL, ",");
    char *shift = strtok(NULL, ",");
    if (imm[0] == '#')
    {
        // then immediate instruction
        tokenise_add_sub_immediate(instruction, add_sub_immdiate_init(operand, rd, rn, imm, shift));
    }
    else
    {
        // then register instruction
        tokenise_add_sub_register(instruction, add_sub_register_init(operand, rd, rn, imm, shift));
    }
}

void parse_logical(unsigned int *instruction, char *operand)
{
    char *rd = strtok(NULL, ",");
    char *rn = strtok(NULL, ",");
    char *rm = strtok(NULL, ",");
    char *shift = strtok(NULL, ",");
    tokenise_logical(instruction, logical_init(operand, rd, rn, rm, shift));
}

void parse_move_wide(unsigned int *instruction, char *operand)
{
    char *rd = strtok(NULL, ",");
    char *imm = strtok(NULL, ",");
    char *shift = strtok(NULL, ",");
    tokenise_move_wide(instruction, move_wide_init(operand, rd, imm, shift));
}

void parse_multiply(unsigned int *instruction, char *operand)
{
    char *rd = strtok(NULL, ",");
    char *rn = strtok(NULL, ",");
    char *rm = strtok(NULL, ",");
    char *ra = strtok(NULL, ",");
    tokenise_multiply(instruction, multiply_init(operand, rd, rn, rm, ra));
}

void parse_load_store(unsigned int *instruction, char *operand, struct symbol_table *table)
{
    char *rt = strtok(NULL, ",");
    char *xn = strtok(NULL, ",");
    char *simm = strtok(NULL, ",");
    if (simm == NULL)
    {
        // then only two parameters (only for ldr)
        assert(strcmp(operand, "ldr") == 0);
        tokenise_load_store_literal(instruction, load_store_literal_init(operand, rt), table);
    }
    else if (simm[0] == '#')
    {
        // then it is post or pre or unsigned offset
        tokenise_load_store(instruction, load_store_init(operand, rt, xn, simm), table);
    }
    else
    {
        // then it is register offset
        char *shift = strtok(NULL, ",");
        tokenise_load_store_shift(instruction, load_store_shift_init(operand, rt, xn, simm, shift), table);
    }
}

// overall parsing for each line
void parse(char *in, int address, unsigned int *instruction, struct symbol_table *table)
{
    // printf("Parsing: %s\n", in);
    char *operand = strtok(in, " ");
    // if operend is analias shown in spec, then change it to the real one
    if (!strcmp(operand, "cmp") || !strcmp(operand, "cmn") || !strcmp(operand, "neg") ||
        !strcmp(operand, "negs") || !strcmp(operand, "tst") || !strcmp(operand, "mvn") ||
        !strcmp(operand, "mov") || !strcmp(operand, "mul") || !strcmp(operand, "meng"))
    {
        to_alias(operand, in);
    }
    // data processing
    if (!strcmp(operand, "add") || !strcmp(operand, "adds") ||
        !strcmp(operand, "sub") || !strcmp(operand, "subs"))
    {
        parse_add_sub(instruction, operand);
    }
    else if (!strcmp(operand, "and") || !strcmp(operand, "ands") ||
             !strcmp(operand, "bic") || !strcmp(operand, "bics") ||
             !strcmp(operand, "eor") || !strcmp(operand, "orr") ||
             !strcmp(operand, "eon") || !strcmp(operand, "orn"))
    {
        parse_logical(instruction, operand);
    }
    else if (!strcmp(operand, "movk") || !strcmp(operand, "movn") || !strcmp(operand, "movz"))
    {
        parse_move_wide(instruction, operand);
    }
    else if (!strcmp(operand, "madd") || !strcmp(operand, "msub"))
    {
        parse_multiply(instruction, operand);
    }
    // branch
    else if (!strcmp(operand, "b") || !strcmp(operand, "b.cond") || !strcmp(operand, "br"))
    {
        char *literal = strtok(NULL, ","); // literal or xn
        tokenise_branch(instruction, branch_init(operand, literal), table);
    }
    // loads and stores
    else if (!strcmp(operand, "ldr") || !strcmp(operand, "str"))
    {
        parse_load_store(instruction, operand, table);
    }
    // special
    else if (!strcmp(operand, "nop"))
    {
        *instruction = 0xd503201f;
    }
    else if (!strcmp(operand, ".int"))
    {
        char *constant = strtok(NULL, ","); // literal or xn
        tokenise_int(instruction, constant_init(operand, constant));
    }
    else
    {
        printf("Error: unknown instruction\n");
    }
}
