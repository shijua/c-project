#include <stdbool.h>
#include <stdint.h>
#ifndef INPUTFORMAT_H
#define INPUTFORMAT_H
struct send_DPI
{
    bool sf;
    uint8_t opc;
    uint8_t opi;
    int32_t operand;
    uint8_t rd;
};

struct send_DPR
{
    bool sf;
    bool M;
    uint8_t opc;
    uint8_t opr;
    uint8_t rm;
    uint8_t operand;
    uint8_t rn;
    uint8_t rd;
};

struct sdtp
{
    bool sf;
    bool L;
    bool U;
    int16_t offset;
    uint8_t xn;
    uint8_t rt;
};

struct loadliteral
{
    bool sf;
    int32_t simm19;
    uint8_t rt;
};

struct send_branch
{
    uint8_t sf;
    int32_t operand;
};
#endif
extern struct send_DPI to_DPI(uint32_t instruction);
extern struct send_DPR to_DPR(uint32_t instruction);
extern struct sdtp to_SDT(uint32_t instruction);
extern struct loadliteral to_LL(uint32_t instruction);
extern struct send_branch to_branch(uint32_t instruction);
