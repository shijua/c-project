#include <stdbool.h>
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

struct send_SDT
{
    bool sf;
    bool L;
    int offset;
    int xn;
    int rt;
};

struct send_LL
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

extern struct send_DPI to_DPI(int instruction);
extern struct send_DPR to_DPR(int instruction);
extern struct send_SDT to_SDT(int instruction);
extern struct send_LL to_LL(int instruction);
extern struct send_branch to_branch(int instruction);
