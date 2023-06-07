#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symboltable.h"

// create the table
struct symbol_table *symbol_table_create(int cap)
{
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
int symbol_table_get(struct symbol_table *table, char *in)
{
    int data = -1;
    for (int i = 0; i < table->len; i++)
        if (strcmp(table->name[i], in) == 0)
            data = table->address[i];
    assert(data != -1);
    return data;
}

// aim for increasing the capacity of the table if it is full
static void grow(struct symbol_table *table)
{
    if (table->len + 1 >= table->cap)
    {
        table->cap += 100;
        table->name = realloc(table->name, table->cap * sizeof(char *));
        table->address = realloc(table->address, table->cap * sizeof(int));
        assert(table->name != NULL);
        assert(table->address != NULL);
    }
}

// push a new name and address into the table
void symbol_table_push(struct symbol_table *table, char *name_in, int address_in)
{
    grow(table);
    table->name[table->len++] = name_in;
    table->address[table->len++] = address_in;
}

// free the table
void symbol_table_free(struct symbol_table *table)
{
    free(table->name);
    free(table->address);
    free(table);
}
