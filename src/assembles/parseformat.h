#ifndef PARSEFORMAT_H
#define PARSEFORMAT_H
// struct for data processing
// shift is optional
struct add_sub_immediate
{
    char *operend, *rd, *rn, *imm, *shift;
};
// shift is optional
struct add_sub_register
{
    char *operend, *rd, *rn, *rm, *shift;
};
struct logical
{
    char *operend, *rd, *rn, *rm, *shift;
};
// shift is optional
struct move_wide
{
    char *operend, *rd, *imm, *shift;
};

struct multiply
{
    char *operend, *rd, *rn, *rm, *ra;
};

// struct for branch
// literal will be Xn in "br", literal in b and b.cond
struct branch
{
    char *operend, *literal;
};

// load and store
// for pre, post, unsigned offset
struct load_store
{
    // simm will be imm in unsigned offset, simm in pre and post
    char *operend, *rt, *xn, *simm;
};
// shift is optional
struct load_store_shift
{
    char *operend, *rt, *xn, *rm, *shift;
};

struct load_store_literal
{
    char *operend, *literal;
};

// struct for .int
struct constant
{
    char *operend, *simm;
};

#endif
extern struct add_sub_immediate add_sub_immdiate_init(char *operend, char *rd, char *rn, char *imm, char *shift);
extern struct add_sub_register add_sub_register_init(char *operend, char *rd, char *rn, char *rm, char *shift);
extern struct logical logical_init(char *operend, char *rd, char *rn, char *rm, char *shift);
extern struct move_wide move_wide_init(char *operend, char *rd, char *imm, char *shift);
extern struct multiply multiply_init(char *operend, char *rd, char *rn, char *rm, char *ra);
extern struct branch branch_init(char *operend, char *literal);
extern struct load_store load_store_init(char *operend, char *rt, char *xn, char *simm);
extern struct load_store_shift load_store_shift_init(char *operend, char *rt, char *xn, char *rm, char *shift);
extern struct load_store_literal load_store_literal_init(char *operend, char *literal);
extern struct constant constant_init(char *operend, char *simm);