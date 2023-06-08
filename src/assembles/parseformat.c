#include "parseformat.h"
struct add_sub_immediate add_sub_immdiate_init(char *opcode, char *rd, char *rn, char *imm, char *shift)
{
    struct add_sub_immediate divide = {opcode, rd, rn, imm, shift};
    return divide;
}

struct add_sub_register add_sub_register_init(char *opcode, char *rd, char *rn, char *rm, char *shift)
{
    struct add_sub_register divide = {opcode, rd, rn, rm, shift};
    return divide;
}

struct logical logical_init(char *opcode, char *rd, char *rn, char *rm, char *shift)
{
    struct logical divide = {opcode, rd, rn, rm, shift};
    return divide;
}

struct move_wide move_wide_init(char *opcode, char *rd, char *imm, char *shift)
{
    struct move_wide divide = {opcode, rd, imm, shift};
    return divide;
}

struct multiply multiply_init(char *opcode, char *rd, char *rn, char *rm, char *ra)
{
    struct multiply divide = {opcode, rd, rn, rm, ra};
    return divide;
}

struct branch branch_init(char *opcode, char *literal)
{
    struct branch divide = {opcode, literal};
    return divide;
}

struct load_store load_store_init(char *opcode, char *rt, char *xn, char *simm)
{
    struct load_store divide = {opcode, rt, xn, simm};
    return divide;
}

struct load_store_register load_store_register_init(char *opcode, char *rt, char *xn, char *rm)
{
    struct load_store_register divide = {opcode, rt, xn, rm};
    return divide;
}

struct load_store_literal load_store_literal_init(char *opcode, char *literal)
{
    struct load_store_literal divide = {opcode, literal};
    return divide;
}

struct constant constant_init(char *opcode, char *simm)
{
    struct constant divide = {opcode, simm};
    return divide;
}
