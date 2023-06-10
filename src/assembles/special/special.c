#include <stdlib.h>
#include "special.h"

void tokenise_int(unsigned int *instruction, struct constant divide) {
    copy_bit(instruction, to_int_2(divide.simm), 0, 31);
}
