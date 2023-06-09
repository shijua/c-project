#include "single_data_transfer.h"
void load_store(unsigned int *instruction, struct load_store divide, struct symbol_table *table) {
    // single data transfer without shift and not literal
    bool isldr = *(divide.opcode + 0) == "l";
    bool sf = check_bit(divide.rt);
    char offset_last = *(divide.simm + strlen(divide.simm) - 1);

    //set common bits
    copy_bit(instruction, 1, 31, 31);
    copy_bit(instruction, 7, 27, 29);
    copy_bit(instruction, register_to_bin(divide.rt), 0, 4);
    copy_bit(instruction, register_to_bin(divide.xn + 1), 9, 5);

    //set specific bits
    if (sf) {
        // if the target register is 64 bit
        copy_bit(instruction, 1, 30, 30);
    } else {
        copy_bit(instruction, 0, 30, 30);
    }
    if (isldr) {
        // if it is load
        copy_bit(instruction, 1, 22, 22);
    } else {
        copy_bit(instruction, 0, 22, 22);
    }
    if (offset_last == "]") {
        if (*(divide.simm + 0) == '#') {
            // unsigned offset
            copy_bit(instruction, 1, 24, 24);
            if (sf) {
                // When Rt is an X-register
                unsigned int imm12 = atoi(divide.simm + 1) / 8;
                copy_bit(instruction, imm12, 10, 21);
            } else {
                // When Rt is an W-register
                unsigned int imm12 = atoi(divide.simm + 1) / 4;
                copy_bit(instruction, imm12, 10, 21);
            }
        } else if (*(divide.simm + 0) == ']') {
            // Zero Unsigned Offset
            copy_bit(instruction, 1, 24, 24);
            copy_bit(instruction, 0, 10, 21);
        } else {
                // register offset
                copy_bit(instruction, 1, 21, 21);
                copy_bit(instruction, 1, 11, 11);
                copy_bit(instruction, 3, 13, 14);
                divide.simm[strlen(divide.simm) - 1] = '\0'; // remove the last ']'
                int xm = (unsigned int) register_to_bin(divide.simm);
                copy_bit(instruction, xm, 16, 20);
            }
        } else if (offset_last == "!") {
        // pre-index
            copy_bit(instruction, 1, 10, 10);
            copy_bit(instruction, 1, 11, 11);
            divide.simm[strlen(divide.simm) - 2] = '\0'; // remove the last ']!'
            int simm9 = atoi(divide.simm + 1);
            simm9 = simm9 < 0 ? simm9 & 0x1FF : simm9;
            copy_bit(instruction, simm9, 12, 20);
        } else {
            // post-index
            copy_bit(instruction, 1, 10, 10);
            copy_bit(instruction, 0, 11, 11);
            int simm9 = atoi(divide.simm + 1);
            simm9 = simm9 < 0 ? simm9 & 0x1FF : simm9;
            copy_bit(instruction, simm9, 12, 20);
        }
}
void load_store_literal(unsigned int *instruction, struct load_store_literal divide, struct symbol_table *table) {
    // load literal
    bool sf = check_bit(divide.rt);
    //set common bits
    copy_bit(instruction, 3, 27, 28);
    copy_bit(instruction, register_to_bin(divide.rt), 0, 4);
    //set specific bits
    if (sf) {
        // if the target register is 64 bit
        copy_bit(instruction, 1, 30, 30);
    } else {
        copy_bit(instruction, 0, 30, 30);
    }
}