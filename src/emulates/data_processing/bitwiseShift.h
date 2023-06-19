#include <stdbool.h>
#include <stdint.h>

extern int64_t logicalShiftLeft(int64_t value, int shiftAmount, bool is64Bit);
extern int64_t logicalShiftRight(int64_t value, int shiftAmount, bool is64Bit);
extern int64_t arithmeticShiftRight(int64_t value, int shiftAmount, bool is64Bit);
extern int64_t rotateRight(int64_t value, int shiftAmount, bool is64Bit);
