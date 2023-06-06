extern char* register_to_bin(char* in);

struct symbol_table
{
    char* name;
    int address;
};

struct symbol_table* symbol_table_init();
void symbol_table_add(struct symbol_table* table, char* name, int address);
int symbol_table_get(struct symbol_table* table, char* name);
void symbol_table_free(struct symbol_table* table);

