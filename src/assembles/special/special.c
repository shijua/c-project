#include <stdlib.h>
#include "special.h"

void tokenise_int(uint32_t *instruction, struct constant divide) {
    copy_bit(instruction, to_int_without_hash(divide.simm), 0, 31);
}
