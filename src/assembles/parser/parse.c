#include "parse.h"
#include "../Util.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

// will be comment later after include other header file for tokenise
void tokenise_add_sub_immediate(int *instruction, struct add_sub_immediate divide);
void tokenise_add_sub_register(int *instruction, struct add_sub_register divide);
void tokenise_logical(int *instruction, struct logical divide);
void tokenise_move_wide(int *instruction, struct move_wide divide);
void tokenise_multiply(int *instruction, struct multiply divide);
void tokenise_branch(int *instruction, struct branch divide, struct symbol_table *table);
void tokenise_load_store(int *instruction, struct load_store divide, struct symbol_table *table);
void tokenise_load_store_shift(int *instruction, struct load_store_shift divide, struct symbol_table *table);
void tokenise_load_store_literal(int *instruction, struct load_store_literal divide, struct symbol_table *table);
void tokenise_int(int *instruction, struct constant divide);

// parsing each specific operation
void parse_add_sub(int *instruction, char *operand, char *in)
{
    char *rd = strtok(in, ", ");
    char *rn = strtok(in, ", ");
    char *imm = strtok(in, ", ");
    char *shift = strtok(in, ", ");
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

void parse_logical(int *instruction, char *operand, char *in)
{
    char *rd = strtok(in, ", ");
    char *rn = strtok(in, ", ");
    char *rm = strtok(in, ", ");
    char *shift = strtok(in, ", ");
    tokenise_logical(instruction, logical_init(operand, rd, rn, rm, shift));
}

void parse_move_wide(int *instruction, char *operand, char *in)
{
    char *rd = strtok(in, ", ");
    char *imm = strtok(in, ", ");
    char *shift = strtok(in, ", ");
    tokenise_move_wide(instruction, move_wide_init(operand, rd, imm, shift));
}

void parse_multiply(int *instruction, char *operand, char *in)
{
    char *rd = strtok(in, ", ");
    char *rn = strtok(in, ", ");
    char *rm = strtok(in, ", ");
    char *ra = strtok(in, ", ");
    tokenise_multiply(instruction, multiply_init(operand, rd, rn, rm, ra));
}

void parse_load_store(int *instruction, char *operand, char *in, struct symbol_table *table)
{
    char *rt = strtok(in, ", ");
    char *xn = strtok(in, ", ");
    char *simm = strtok(in, ", ");
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
        char *shift = strtok(in, ", ");
        tokenise_load_store_shift(instruction, load_store_shift_init(operand, rt, xn, simm, shift), table);
    }
}

// overall parsing for each line
void parse(char *in, int address, int *instruction, struct symbol_table *table)
{
    char *operand = strtok(in, " ");
    // if operend is analias shown in spec, then change it to the real one
    if (strcmp(operand, "cmp") || strcmp(operand, "cmn") || strcmp(operand, "neg") ||
        strcmp(operand, "negs") || strcmp(operand, "tst") || strcmp(operand, "mvn") ||
        strcmp(operand, "mov") || strcmp(operand, "mul") || strcmp(operand, "meng"))
    {
        to_alias(operand, in);
    }
    // data processing
    if (strcmp(operand, "add") || strcmp(operand, "adds") ||
        strcmp(operand, "sub") || strcmp(operand, "subs"))
    {
        parse_add_sub(instruction, operand, in);
    }
    else if (strcmp(operand, "and") || strcmp(operand, "ands") ||
             strcmp(operand, "bic") || strcmp(operand, "bics") ||
             strcmp(operand, "eor") || strcmp(operand, "orr") ||
             strcmp(operand, "eon") || strcmp(operand, "orn"))
    {
        parse_logical(instruction, operand, in);
    }
    else if (strcmp(operand, "movk") || strcmp(operand, "movn") || strcmp(operand, "movz"))
    {
        parse_move_wide(instruction, operand, in);
    }
    else if (strcmp(operand, "madd") || strcmp(operand, "msub"))
    {
        parse_multiply(instruction, operand, in);
    }
    // branch
    else if (strcmp(operand, "b") || strcmp(operand, "b.cond") || strcmp(operand, "br"))
    {
        tokenise_branch(instruction, branch_init(operand, in), table);
    }
    // loads and stores
    else if (strcmp(operand, "ldr") || strcmp(operand, "str"))
    {
        parse_load_store(instruction, operand, in, table);
    }
    // special
    else if (strcmp(operand, "nop"))
    {
        *instruction = 0xd503201f;
    }
    else if (strcmp(operand, ".int"))
    {
        tokenise_int(instruction, constant_init(operand, in));
    }
    else
    {
        printf("Error: unknown instruction\n");
        // exit(EXIT_FAILURE);
    }
}

int main(int argc, char const *argv[])
{
    parse("add x0, x1, x2", 0, NULL, NULL);
    return 0;
}
