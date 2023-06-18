#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

struct symbol_table {
    char **name; // array of char pointers
    int *address; // array of ints
    int cap; // current allocated capacity
    int len; // number of element stored elements stored (pre: len <= cap)
};
#endif

extern struct symbol_table *symbol_table_create(int cap);

extern int symbol_table_get(struct symbol_table *table, char *in);

extern void symbol_table_push(struct symbol_table *table, char *name, int address);

extern void symbol_table_free(struct symbol_table *table);
