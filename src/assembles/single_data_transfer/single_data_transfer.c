#include "single_data_transfer.h"
/*
void load_store(unsigned int *instruction, struct load_store divide, struct symbol_table *table) {
    // single data transfer without shift and not literal
    bool isldr = *(divide.operend + 0) == "l";
    bool rt = check_bit(divide.rt);
    char offset_last = *(divide.simm + strlen(divide.simm) - 1);
    if (rt) {
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
        // unsigned offset
        copy_bit(instruction, 1, 24, 24);
    } else if (offset_last == "!") {
        // pre-index
        copy_bit(instruction, 1, 11, 11);
    } else {
        // post-index
        copy_bit(instruction, 0, 11, 11);
    }

}
*/