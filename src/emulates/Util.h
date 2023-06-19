#ifndef UTIL_H
#define UTIL_H
#include <stdbool.h>
#include <stdint.h>
#define MAX_MEMORY 2100000
struct PSTATE
{
    bool N, Z, C, V;
};

struct Registers {
    // int R0 to R30 for 31 genreal purpose registers
    int64_t general[31];
    // Special registers
    int64_t PC, ZR, SP;
    // processor state register
    struct PSTATE pstate;
};
#endif

extern uint32_t getBitRange(int num, int x, int y);
extern uint32_t get_bit(int startBit, int numBits, uint32_t number);
extern uint64_t get_bitl(int startBit, int numBits, uint64_t number);
extern int sget_bit(int startBit, int numBits, int number);
extern bool hasCarryBorrow(int64_t a, int64_t b, bool is_64, bool is_sub);
extern bool overflow (int64_t a, int64_t b , bool is_64);
