#include <stdbool.h>
#include <string.h>
struct send_DPI
{
    bool sf;
    char opc[2];
    char opi[3];
    char operand[18];
    char rd[5];
};

struct send_DPR
{
    bool sf;
    char opc[2];
    char opr[3];
    char rm[5];
    char operand[6];
    char rn[5];
    char rd[5];
};

struct send_SDT
{
    bool sf;
    bool L;
    char offset[12];
    char xn[5];
    char rt[5];
};

struct send_LL
{
    bool sf;
    char simm19[19];
    char rt[5];
};

struct send_branch
{
    char sf[2];
    char operand[26];
};

struct send_DPI to_DPI(char instruction[32]) {
    struct send_DPI divide;
    divide.sf = instruction[0] - '0';
    memcpy(divide.opc, instruction + 1, 2);
    memcpy(divide.opi, instruction + 6, 3);
    memcpy(divide.operand, instruction + 9, 18);
    memcpy(divide.rd, instruction + 27, 5);
    return divide;
}

struct send_DPR to_DPR(char instruction[32]) {
    struct send_DPR divide;
    divide.sf = instruction[0] - '0';
    memcpy(divide.opc, instruction + 1, 2);
    memcpy(divide.opr, instruction + 6, 3);
    memcpy(divide.rm, instruction + 9, 5);
    memcpy(divide.operand, instruction + 14, 6);
    memcpy(divide.rn, instruction + 22, 5);
    memcpy(divide.rd, instruction + 27, 5);
    return divide;
}

struct send_SDT to_SDT(char instruction[32]) {
    struct send_SDT divide;
    divide.sf = instruction[1] - '0';
    divide.L = instruction[9] - '0';
    memcpy(divide.offset, instruction + 10, 12);
    memcpy(divide.xn, instruction + 22, 5);
    memcpy(divide.rt, instruction + 27, 5);
    return divide;
}

struct send_LL to_LL(char instruction[32]) {
    struct send_LL divide;
    divide.sf = instruction[1] - '0';
    memcpy(divide.simm19, instruction + 8, 19);
    memcpy(divide.rt, instruction + 27, 5);
    return divide;
}

struct send_branch to_branch(char instruction[32]) {
    struct send_branch divide;
    memcpy(divide.sf, instruction, 2);
    memcpy(divide.operand, instruction + 6, 26);
    return divide;
}

