#include <stdio.h>
int get_bit(int startbit, int length, int instruction) {
    int mask = 0;
    for (int i = 0; i < length; i++) {
        mask <<= 1;
        mask += 1;
    }
    // printf("mask: %d\n", mask);
    return (instruction >> (startbit - length + 1)) & mask;
}


// int main(int argc, char const *argv[])
// {
//     get_bit(28, 4, 335544322);
//     return 0;
// }
