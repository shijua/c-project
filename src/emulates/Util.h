#ifndef UTIL_H
#define UTIL_H
#include <stdbool.h>
#define MAX_MEMORY 2100000
struct PSTATE
{
    bool N, Z, C, V;
};

struct Registers {
    // int R0 to R30 for 31 genreal purpose registers
    long long general[31];
    // Special registers
    long long PC, ZR, SP;
    // processor state register
    struct PSTATE pstate;
};
#endif

extern unsigned int getBitRange(int num, int x, int y);
extern unsigned int get_bit(int startBit, int numBits, unsigned int number);
extern unsigned long long get_bitl(int startBit, int numBits, unsigned long long number);
extern int sget_bit(int startBit, int numBits, int number);
extern bool hasCarryBorrow(long long a, long long b, bool is_64, bool is_sub);
extern bool overflow (long long a, long long b , bool is_64);
