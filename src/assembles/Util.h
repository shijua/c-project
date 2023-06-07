#ifndef UTIL_H
#define UTIL_H
// TODO symbolTable
struct node{
    char* name;
    int address;
};
struct symbol_table
{
    struct node* node;
    // next
    struct symbol_table* next;
};
#endif
// a function use for dealing with shift
extern struct symbol_table* symbol_table_init();
extern void symbol_table_add(struct symbol_table* table, char* name, int address);
extern int symbol_table_get(struct symbol_table* table, char* name);

extern char* register_to_bin(char* in);
extern void copy_bit(unsigned int* in, unsigned int change, int start, int end);
// check in 32 bit or 64 bit;
extern char check_bit(char* in, int bit);