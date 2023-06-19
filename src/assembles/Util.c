#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "Util.h"

// convert a register string into binary
// format: ror or x[num] or w[num] (ex: x10 w0)
char register_to_bin(char *in) {
    // zero register gives 11111;
    if (!strcmp("rzr", in) || !strcmp("xzr", in))
        return 0x1F;
        // otherwise check the rest characters(except the first x or w)
        // convert string into integer
    else
        return atoi(in + 1);

}

void copy_bit(uint32_t *in, uint32_t change, uint8_t start, uint8_t end) {
    // get the length of change
    assert(end >= start);
    uint8_t len = end - start + 1;
    // ensure change is in the right range (for supporting negative number)
    change &= ((1L << len) - 1);
    // get the mask (11110001 for example)
    uint32_t mask = ~(((1U << len) - 1) << start);
    // shift the change to the right position
    change = change << start;
    // firstly mask then replace 0 position with change
    *in = (*in & mask) | change;
}

// check it is in 32 or 64 bit
bool check_bit(char *in) {
    // 32 return 0 and 64 return 1
    if (in[0] == 'w') return 0;
    return 1;
}

// get the substring of a string
// remember to free the returned pointer
// ex: substring("hello world", 0, 5) gives "hell"
char *substring(char *str, uint8_t start, uint8_t end) {
    int len = end - start;
    char *substr = malloc(len + 1);
    strncpy(substr, str + start, len);
    substr[len] = '\0';
    return substr;
}

int to_int(char *in) {
    if (!strcmp(substring(in, 0, 3), "#0x") || !strcmp(substring(in, 0, 3), "#0X"))
        return strtol(in + 3, NULL, 16);
    int x = atoi(in + 1);
    return x;
}

int to_int_without_hash(char *in) {  // without #
    if (!strcmp(substring(in, 0, 2), "0x") || !strcmp(substring(in, 0, 2), "0X"))
        return strtol(in + 2, NULL, 16);
    int x = atoi(in);
    return x;
}

bool is_label(char *literal) {
    char a = literal[0];
    if (('a' <= a && a <= 'z') || ('A' <= a && a <= 'Z')) {
        return true;
    }
    return false;
}
