#include "parseformat.h"
struct add_sub_immediate add_sub_immdiate_init(char *operend, char *rd, char *rn, char *imm, char *shift)
{
    struct add_sub_immediate divide = {operend, rd, rn, imm, shift};
    return divide;
}

struct add_sub_register add_sub_register_init(char *operend, char *rd, char *rn, char *rm, char *shift)
{
    struct add_sub_register divide = {operend, rd, rn, rm, shift};
    return divide;
}

struct logical logical_init(char *operend, char *rd, char *rn, char *rm, char *shift)
{
    struct logical divide = {operend, rd, rn, rm, shift};
    return divide;
}

struct move_wide move_wide_init(char *operend, char *rd, char *imm, char *shift)
{
    struct move_wide divide = {operend, rd, imm, shift};
    return divide;
}

struct multiply multiply_init(char *operend, char *rd, char *rn, char *rm, char *ra)
{
    struct multiply divide = {operend, rd, rn, rm, ra};
    return divide;
}

struct branch branch_init(char *operend, char *literal)
{
    struct branch divide = {operend, literal};
    return divide;
}

struct load_store load_store_init(char *operend, char *rt, char *xn, char *simm)
{
    struct load_store divide = {operend, rt, xn, simm};
    return divide;
}

struct load_store_shift load_store_shift_init(char *operend, char *rt, char *xn, char *rm, char *shift)
{
    struct load_store_shift divide = {operend, rt, xn, rm, shift};
    return divide;
}

struct load_store_literal load_store_literal_init(char *operend, char *literal)
{
    struct load_store_literal divide = {operend, literal};
    return divide;
}

struct constant constant_init(char *operend, char *simm)
{
    struct constant divide = {operend, simm};
    return divide;
}