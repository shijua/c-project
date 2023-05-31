#ifndef INPUTFORMAT_H
#define INPUTFORMAT_H
struct send_DPI
{
    bool sf;
    int opc;
    int opi;
    int operand;
    int rd;
};

struct send_DPR
{
    bool sf;
    int opc;
    int opr;
    int rm;
    int operand;
    int rn;
    int rd;
};

struct sdtp
{
    bool sf;
    bool L;
    bool U;
    int offset;
    int xn;
    int rt;
};

struct loadliteral
{
    bool sf;
    int simm19;
    int rt;
};

struct send_branch
{
    int sf;
    int operand;
};
#endif
extern struct send_DPI to_DPI(int instruction);
extern struct send_DPR to_DPR(int instruction);
extern struct sdtp to_SDT(int instruction);
extern struct loadliteral to_LL(int instruction);
extern struct send_branch to_branch(int instruction);
