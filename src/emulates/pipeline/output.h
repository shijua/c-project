#include "../Util.h"
extern void output_general(int64_t* general_register_ptr, FILE* fp);
extern void output_Pstate(bool* pstate_ptr, FILE* fp);
extern void output_memory(char* memory, FILE* fp);
extern void output(struct Registers* register_ptr, char* memory, FILE* fp);
