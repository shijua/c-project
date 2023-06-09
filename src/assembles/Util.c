#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Util.h"

// convert a register string into binary
// format: ror or x[num] or w[num] (ex: x10 w0)
char register_to_bin(char *in) {
    // zero register gives 11111;
    if (!strcmp("rzr", in)) 
        return 0x1F;
    // otherwise check the rest characters(except the first x or w)
    // convert string into integer
    else 
        return atoi(in + 1);
    
}

// copy the bit from change into *in
// ex: 10011001 in *in, 101 in change, start = 2, end = 4 gives 100 101 01(middle is changing)
//void copy_bit(unsigned int *in, unsigned int change, int start, int end) {
//    // get the length of change
//    int len = end - start + 1;
//    // ensure change is in the right range
//    assert(change < (1 << len));
//    // get the mask (11110001 for example above)
//    int mask = ~0 - (((1 << len) - 1) << start);
//    // shift the change to the right position
//    change = change << start;
//    // firstly mask then replace 0 position with change
//    *in = (*in & mask) | change;
//}

void copy_bit(unsigned int *in, unsigned int change, int start, int end) {
    // get the length of change
    int len = end - start + 1;

    // ensure change is in the right range
    assert(change < (1U << len));

    // get the mask (11110001 for example)
    unsigned int mask = ~(((1U << len) - 1) << start);

    // shift the change to the right position
    change = change << start;

    // firstly mask then replace 0 position with change
    *in = (*in & mask) | change;
}



// check it is in 32 or 64 bit
bool check_bit(char *in) {
    // 32 return 0 and 64 return 1
    if (*(in + 0) == 'w') return 0;
    return 1;
}

// get the substring of a string
// remember to free the returned pointer
char* substring(char* str, int start, int end) {
    int len = end - start;
    char* substr = malloc(len + 1);
    strncpy(substr, str + start, len);
    substr[len] = '\0';
    return substr;
}
unsigned int to_num(const char *hex_string) {
    char *end;
    long number = strtol(hex_string, &end, 16);
    return (unsigned int) number;
}


// #include <stdio.h>
// int main() {
//     // test copybit
//     unsigned int in = 0x99;
//     copy_bit(&in, 0x0, 0, 0);
//     printf("%x\n", in);
//     unsigned int in2 = 0x99;
//     copy_bit(&in2, 0x5, 2, 4);
//     printf("%x\n", in2); // expect 0x95
//     // test register to bin
//     printf("%x\n", register_to_bin("x10"));
//     printf("%x\n", register_to_bin("w0"));
//     printf("%x\n", register_to_bin("rzr"));
//     // test check bit
//     printf("%d\n", check_bit("w0"));
//     printf("%d\n", check_bit("x0"));
//     // test substring
//     char* substr = substring("hello world", 0, 5);
//     printf("%s\n", substr);
//     free(substr);
//     return 0;
// }
