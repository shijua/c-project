#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symboltable.h"

// create the table
struct symbol_table *symbol_table_create(int cap) {
    struct symbol_table *table = malloc(sizeof(struct symbol_table));
    assert(table != NULL);
    table->name = malloc(cap * sizeof(char *));
    table->address = malloc(cap * sizeof(int));
    assert(table->name != NULL);
    assert(table->address != NULL);
    table->len = 0;
    table->cap = cap;
    return table;
}

// get relative address by name (guaranteed to be in the table)
int symbol_table_get(struct symbol_table *table, char *in) {
    int data = -1;
    for (int i = 0; i < table->len; i++)
        if (!strcmp(table->name[i], in))
            data = table->address[i];
    assert(data != -1);
    return data;
}

// aim for increasing the capacity of the table if it is full
static void grow(struct symbol_table *table) {
    if (table->len + 1 >= table->cap) {
        table->cap += 100;
        table->name = realloc(table->name, table->cap * sizeof(char *));
        table->address = realloc(table->address, table->cap * sizeof(int));
        assert(table->name != NULL);
        assert(table->address != NULL);
    }
}

// push a new name and address into the table
void symbol_table_push(struct symbol_table *table, char *name_in, uint32_t address_in) {
    grow(table);
    // allocate memory for the name
    char *temp = malloc(strlen(name_in) + 1);
    strcpy(temp, name_in);
    temp[strlen(name_in)] = '\0';
    table->name[table->len] = temp;
    table->address[table->len++] = address_in;
}

// free the table
void symbol_table_free(struct symbol_table *table) {
    for (int i = 0; i < table->len; i++)
        free(table->name[i]);
    free(table->name);
    free(table->address);
    free(table);
}
