#ifndef PARSEFORMAT_H
#define PARSEFORMAT_H
// struct for data processing
// shift is optional
struct add_sub_immediate
{
    char *opcode, *rd, *rn, *imm, *shift;
};
// shift is optional
struct add_sub_register
{
    char *opcode, *rd, *rn, *rm, *shift;
};
struct logical
{
    char *opcode, *rd, *rn, *rm, *shift;
};
// shift is optional
struct move_wide
{
    char *opcode, *rd, *imm, *shift;
};

struct multiply
{
    char *opcode, *rd, *rn, *rm, *ra;
};

// struct for branch
// literal will be Xn in "br", literal in b and b.cond
struct branch
{
    char *opcode, *literal;
};

// load and store
// for pre, post, unsigned offset
struct load_store
{
    // simm will be imm in unsigned offset, simm in pre and post
    char *opcode, *rt, *xn, *simm;
};

struct load_store_literal
{
    char *opcode, *rt, *literal;
};

// struct for .int
struct constant
{
    char *opcode, *simm;
};

#endif
extern struct add_sub_immediate add_sub_immdiate_init(char *opcode, char *rd, char *rn, char *imm, char *shift);
extern struct add_sub_register add_sub_register_init(char *opcode, char *rd, char *rn, char *rm, char *shift);
extern struct logical logical_init(char *opcode, char *rd, char *rn, char *rm, char *shift);
extern struct move_wide move_wide_init(char *opcode, char *rd, char *imm, char *shift);
extern struct multiply multiply_init(char *opcode, char *rd, char *rn, char *rm, char *ra);
extern struct branch branch_init(char *opcode, char *literal);
extern struct load_store load_store_init(char *opcode, char *rt, char *xn, char *simm);
extern struct load_store_literal load_store_literal_init(char *opcode, char *rt, char *literal);
extern struct constant constant_init(char *opcode, char *simm);
