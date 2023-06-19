#include "single_data_transfer.h"
void tokenise_load_store(uint32_t *instruction, struct load_store divide, struct symbol_table *table) {
    // single data transfer without shift and not literal
    bool isldr = divide.opcode[0] == 'l';
    bool sf = check_bit(divide.rt);
    bool is_zero = false;
    char offset_last;
    if (divide.simm) {
        offset_last = *(divide.simm + strlen(divide.simm) - 1);
    } else {
        is_zero = true;
    }


    //set common bits
    copy_bit(instruction, 1, 31, 31);
    copy_bit(instruction, 7, 27, 29);
    copy_bit(instruction, register_to_bin(divide.rt), 0, 4);
    copy_bit(instruction, register_to_bin(divide.xn + 1), 5, 9);

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

    if (is_zero) {
        // Zero Unsigned Offset
        copy_bit(instruction, 1, 24, 24);
        copy_bit(instruction, 0, 10, 21);
        char *newreg = divide.xn + 1; // remove the front [
        newreg[strlen(newreg) - 1] = '\0'; // remove the last ]
        copy_bit(instruction, register_to_bin(newreg), 5, 9); // reset xn
    } else if (offset_last == ']') {
        if (divide.simm[0] == '#') {
            // unsigned offset
            copy_bit(instruction, 1, 24, 24);
            if (sf) {
                // When Rt is an X-register
                uint32_t imm12 = to_int(divide.simm) / 8;
                copy_bit(instruction, imm12, 10, 21);
            } else {
                // When Rt is an W-register
                uint32_t imm12 = to_int(divide.simm) / 4;
                copy_bit(instruction, imm12, 10, 21);
            }
        } else {
                // register offset
                copy_bit(instruction, 1, 21, 21);
                copy_bit(instruction, 1, 11, 11);
                copy_bit(instruction, 3, 13, 14);
                divide.simm[strlen(divide.simm) - 1] = '\0'; // remove the last ']'
                uint8_t xm = register_to_bin(divide.simm);
                copy_bit(instruction, xm, 16, 20);
            }
        } else if (offset_last == '!') {
        // pre-index
            copy_bit(instruction, 1, 10, 10);
            copy_bit(instruction, 1, 11, 11);
            divide.simm[strlen(divide.simm) - 2] = '\0'; // remove the last ']!'
            int16_t simm9 = to_int(divide.simm);
            simm9 = simm9 < 0 ? simm9 & 0x1FF : simm9;
            copy_bit(instruction, simm9, 12, 20);
        } else {
            // post-index
            copy_bit(instruction, 1, 10, 10);
            copy_bit(instruction, 0, 11, 11);
            int16_t simm9 = to_int(divide.simm);
            simm9 = simm9 < 0 ? simm9 & 0x1FF : simm9;
            copy_bit(instruction, simm9, 12, 20);
        }
}
void tokenise_load_store_literal(uint32_t *instruction, struct load_store_literal divide, struct symbol_table *table, u_int32_t address) {
    // load literal
    bool sf = check_bit(divide.rt);
    int32_t simm19;

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
    if (divide.literal[0] != '#') {
        // if it is a label
        uint32_t literal_address = symbol_table_get(table, divide.literal);
        simm19 = (literal_address - address) / 4;
    } else {
        // if it is an unsigned immediate address
        uint32_t target_address = to_int(divide.literal); // get the literal address (remove the front #)
        simm19 = (target_address - address) / 4;
    }
    simm19 = simm19 < 0 ? simm19 & 0x7FFFF : simm19; // get corresponding unsigned 19 bit
    copy_bit(instruction, simm19, 5, 23);
}
