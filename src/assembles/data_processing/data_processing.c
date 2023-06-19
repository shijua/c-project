#include "data_processing.h"
#include "../Util.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <printf.h>

void copy_arithm_opc(uint32_t *instr, char *opcode) {
    if (!strcmp(opcode, "add")) {
        copy_bit(instr, 0, 29, 30);
    }
    if (!strcmp(opcode, "adds")) {
        copy_bit(instr, 1, 29, 30);
    }
    if (!strcmp(opcode, "sub")) {
        copy_bit(instr, 2, 29, 30);
    }
    if (!strcmp(opcode, "subs")) {
        copy_bit(instr, 3, 29, 30);
    }
}

void copy_opr(uint32_t *instr, char *shift, bool is_arith, bool N) {
    char* type = substring(shift, 0, 3);
    if (!strcmp(type, "lsl")) {
        copy_bit(instr, 8 * is_arith + N, 21, 24);//shift type : lsl
    } else if (!strcmp(type, "lsr")) {
        copy_bit(instr, 8 * is_arith + N + 2, 21, 24);//shift type : lsr
    } else if (!strcmp(type, "asr")) {
        copy_bit(instr, 8 * is_arith + N + 4, 21, 24);//shift type : asr
    } else if (!strcmp(type, "ror") && !is_arith) {
        copy_bit(instr, 8 * is_arith + N + 6, 21, 24);//shift type : ror
    }
}

extern void tokenise_add_sub_immediate(uint32_t *instr, struct add_sub_immediate divide) {
    copy_arithm_opc(instr, divide.opcode); //opc
    // if first is not rzr then get sf by rn else rm
    // pre: at most one rzr
    if (strcmp(divide.rd, "rzr")) {
        copy_bit(instr, (check_bit(divide.rd)), 31, 31);//sf
    } else {
        copy_bit(instr, (check_bit(divide.rn)), 31, 31);//sf
    }
    copy_bit(instr, 4, 26, 28);//op0
    copy_bit(instr, 2, 23, 25);//opi
    copy_bit(instr, register_to_bin(divide.rd), 0, 4);//rd
    if (divide.shift == NULL || to_int(divide.shift + 4) == 0)
        copy_bit(instr, 0, 22, 22);
    else
        copy_bit(instr, 1, 22, 22);//sh
    //divide.imm + 3 is for eliminating the prefix "#0x" of imm
    //strtol is turns a string representing hex to a int.
    copy_bit(instr, to_int(divide.imm), 10, 21); //imm12
    copy_bit(instr, register_to_bin(divide.rn), 5, 9); //rn

}

extern void tokenise_add_sub_register(uint32_t *instr, struct add_sub_register divide) {
    copy_bit(instr, register_to_bin(divide.rd), 0, 4);//rd
    copy_bit(instr, register_to_bin(divide.rn), 5, 9); //rn
    copy_bit(instr, register_to_bin(divide.rm), 16, 20); //rm
    copy_bit(instr, (check_bit(divide.rn)), 31, 31);//sf
    copy_arithm_opc(instr, divide.opcode); // opc
    copy_bit(instr, 5, 25, 28);//M and bit 25 to 27 are constant, being 0101 which is equal to 5
    if (divide.shift != NULL) {
        copy_opr(instr, divide.shift, 1, 0); //opr

        copy_bit(instr, to_int(divide.shift + 4), 10, 15);//operand
    } else {
        copy_bit(instr, 8, 21, 24);//opr
        copy_bit(instr, 0, 10, 15);//operand
    }

}

extern void tokenise_logical(uint32_t *instr, struct logical divide) {
    copy_bit(instr, register_to_bin(divide.rd), 0, 4);//rd
    copy_bit(instr, register_to_bin(divide.rn), 5, 9); //rn
    copy_bit(instr, register_to_bin(divide.rm), 16, 20); //rm
    // if first is not rzr then get sf by rn else rm
    // pre: at most one rzr
    if (strcmp(divide.rd, "rzr")) {
        copy_bit(instr, (check_bit(divide.rd)), 31, 31);//sf
    } else {
        copy_bit(instr, (check_bit(divide.rn)), 31, 31);//sf
    }
    copy_bit(instr, 5, 25, 28);//M and bit 25 to 27 are constant, being 0101 which is equal to 5
    bool N = 0;
    if (!strcmp(divide.opcode, "and") || !strcmp(divide.opcode, "bic")) {
        copy_bit(instr, 0, 29, 30);
        if (!strcmp(divide.opcode, "bic")) N = 1;
    }
    if (!strcmp(divide.opcode, "orr") || !strcmp(divide.opcode, "orn")) {
        copy_bit(instr, 1, 29, 30);
        if (!strcmp(divide.opcode, "orn")) N = 1;
    }
    if (!strcmp(divide.opcode, "eon") || !strcmp(divide.opcode, "eor")) {
        copy_bit(instr, 2, 29, 30);
        if (!strcmp(divide.opcode, "eon")) N = 1;
    }
    if (!strcmp(divide.opcode, "ands") || !strcmp(divide.opcode, "bics")) {
        copy_bit(instr, 3, 29, 30);
        if (!strcmp(divide.opcode, "bics")) N = 1;
    }

    if (divide.shift != NULL) {
        copy_opr(instr, divide.shift, 0, N); //opr
        copy_bit(instr, to_int(divide.shift + 4), 10, 15);//operand
    } else {
        copy_bit(instr, N, 21, 24);//opr
        copy_bit(instr, 0, 10, 15);//operand
    }
}

void tokenise_move_wide(uint32_t *instruction, struct move_wide divide) {
    //opi
    copy_bit(instruction, 5, 23, 25);
    if (strcmp(divide.opcode, "movn") == 0) {
        copy_bit(instruction, 0, 29, 30);
    } else if (strcmp(divide.opcode, "movz") == 0) {
        copy_bit(instruction, 2, 29, 30);
    } else if (strcmp(divide.opcode, "movk") == 0) {
        copy_bit(instruction, 3, 29, 30);
    } else {
        printf("Invalid opcode\n");
    }
    //sim16
    copy_bit(instruction, to_int(divide.imm), 5, 20);
    //rd
    copy_bit(instruction, register_to_bin(divide.rd), 0, 4);
    //26-28
    copy_bit(instruction, 4, 26, 28);
    copy_bit(instruction, (check_bit(divide.rd)), 31, 31);//sf
    if (divide.shift != NULL) {
        assert (to_int(divide.shift + 4) % 16 == 0);
        copy_bit(instruction, to_int(divide.shift + 4) / 16, 21, 22);
    } else {
        copy_bit(instruction, 0, 21, 22);
    }
}

void tokenise_multiply(uint32_t *instr, struct multiply divide) {
    copy_bit(instr, register_to_bin(divide.rd), 0, 4);//rd
    copy_bit(instr, register_to_bin(divide.rn), 5, 9); //rn
    copy_bit(instr, register_to_bin(divide.rm), 16, 20); //rm
    copy_bit(instr, register_to_bin(divide.ra), 10, 14); //ra
    copy_bit(instr, (check_bit(divide.rd)), 31, 31);//sf
    copy_bit(instr, 216, 21, 30);//bits from 21 to 30 are constant
    if (!strcmp(divide.opcode, "madd")) {
        copy_bit(instr, 0, 15, 15);
    } else {
        copy_bit(instr, 1, 15, 15);
    }
}


