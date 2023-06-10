#include <stdbool.h>
extern char register_to_bin(char *in);
extern void copy_bit(unsigned int *in, unsigned int change, int start, int end);
// check in 32 bit or 64 bit;
extern bool check_bit(char *in);
extern char *substring(char *str, int start, int end);
unsigned int to_num(const char *hex_string);
int to_int (char * in);
int to_int_2(char * in);
