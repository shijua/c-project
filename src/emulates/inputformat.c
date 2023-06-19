#include "Util.h"
#include "inputformat.h"

struct send_DPI to_DPI(uint32_t instruction) {
    struct send_DPI divide;
    divide.sf = get_bit(31, 1, instruction);
    divide.opc = get_bit(30, 2, instruction);
    divide.opi = get_bit(25, 3, instruction);
    divide.operand = get_bit(22, 18, instruction);
    divide.rd = get_bit(4, 5, instruction);
    return divide;
}

struct send_DPR to_DPR(uint32_t instruction) {
    struct send_DPR divide;
    divide.sf = get_bit(31, 1, instruction);
    divide.opc = get_bit(30, 2, instruction);
    divide.M = get_bit(28, 1, instruction);
    divide.opr = get_bit(24, 4, instruction);
    divide.rm = get_bit(20, 5, instruction);
    divide.operand = get_bit(15, 6, instruction);
    divide.rn = get_bit(9, 5, instruction);
    divide.rd = get_bit(4, 5, instruction);
    return divide;
}

struct sdtp to_SDT(uint32_t instruction) {
    struct sdtp divide;
    divide.sf = get_bit(30, 1, instruction);
    divide.U = get_bit(24, 1, instruction);
    divide.L = get_bit(22, 1, instruction);
    divide.offset = get_bit(21, 12, instruction);
    divide.xn = get_bit(9, 5, instruction);
    divide.rt = get_bit(4, 5, instruction);
    return divide;
}

struct loadliteral to_LL(uint32_t instruction) {
    struct loadliteral divide;
    divide.sf = get_bit(30, 1, instruction);
    divide.simm19 = sget_bit(23, 19, instruction);
    divide.rt = get_bit(4, 5, instruction);
    return divide;
}

struct send_branch to_branch(uint32_t instruction) {
    struct send_branch divide;
    divide.sf = get_bit(31, 2, instruction);
    divide.operand = sget_bit(25, 26, instruction);
    return divide;
}

