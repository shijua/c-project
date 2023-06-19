#include <stdbool.h>
#include <stdint.h>

extern char register_to_bin(char *in);

extern void copy_bit(uint32_t *in, uint32_t change, uint8_t start, uint8_t end);

// check in 32 bit or 64 bit;
extern bool check_bit(char *in);

extern char *substring(char *str, uint32_t start, uint32_t end);

extern int to_int(char *in);

extern int to_int_without_hash(char *in);

extern bool is_label(char *literal);
