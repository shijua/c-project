#include <stdbool.h>
extern void output_general(long long* general_register_ptr);
extern void output_Pstate(bool* pstate_ptr);
extern void output_memory(int* memory);
extern void output(struct Registers* register_ptr, int* memory);
