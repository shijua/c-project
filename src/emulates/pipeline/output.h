extern void output_general(long long* general_register_ptr, FILE* fp);
extern void output_Pstate(bool* pstate_ptr, FILE* fp);
extern void output_memory(int* memory, FILE* fp);
extern void output(struct Registers* register_ptr, int* memory, FILE* fp);
