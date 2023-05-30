#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include "Main.c"

struct pre {
    char type[2];
    char operand[26];
};

struct pra {
    char type[2];
    int simM26;
    int Xn;
    int SimM19;
    int cond;
};

int to_int(char* op, int start, int end) {
    if (start > end || end > strlen(op)) {
        return 0;
    }

    char substring[end - start + 2];
    strncpy(substring, &op[start], end - start + 1);
    substring[end - start + 1] = '\0';

    char *endPtr;
    long value = strtol(substring, &endPtr, 10);

    if (endPtr == substring || *endPtr != '\0' || errno == ERANGE) { // if the string is not a number  from ChatGPT lol
        return 0;
    }

    if (value > INT_MAX || value < INT_MIN) {
        return 0;
    }

    return (int)value;
}

void init(struct pra *pra, struct pre pre) {
    pra->type[0] = pre.type[0];
    pra->type[1] = pre.type[1];
    pra->simM26 = to_int(pre.operand, 0, 25);
    pra->Xn = to_int(pre.operand, 5, 9);
    pra->SimM19 = to_int(pre.operand, 5, 23);
}

bool conditional_branch(struct Registers * reg, unsigned int condition) {
    if (condition == 0b0000 && reg->pstate.Z == 1)
        return true;

    if (condition == 0b0001 && reg->pstate.Z == 0)
        return true;

    if (condition == 0b1010 && reg->pstate.N == 1)
        return true;

    if (condition == 0b1011 && reg->pstate.N == 0)
        return true;

    if (condition == 0b1100 && reg->pstate.Z == 0 && reg->pstate.N == reg->pstate.V)
        return true;

    if (condition == 0b1101 && !(reg->pstate.Z == 0 && reg->pstate.N == reg->pstate.V))
        return true;

    if (condition == 0b1110)
        return true;

    return false;
}

void Branch(struct pre *pre, struct Registers *registers_ptr) {
    struct pra pra;
    init(&pra, *pre);  // You should pass *pre instead of pre

    if (strcmp(pra.type, "00") == 0) {
        registers_ptr->PC += pra.simM26;
    }
    else if (strcmp(pra.type, "11") == 0) {
        registers_ptr->PC = registers_ptr->general[pra.Xn];
    }
    else if (strcmp(pra.type, "01") == 0) {
        if (conditional_branch(registers_ptr, pra.cond)) {
            registers_ptr->PC += pra.SimM19;
        }
    }
}
