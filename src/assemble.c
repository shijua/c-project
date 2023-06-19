#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "assembles/parser/input.h"
#include "assembles/symboltable.h"
#include "assembles/parser/parser.h"
int main(int argc, char **argv) {
    if (argc != 3) {
        printf("inputformat: ./assemble <.s file> <.bin file>\n");       
        return 0;
    }
    char *buffer = readfile(argv[1]);
    int file_size = strlen(buffer) - 1;

    // first pass
    // construct a symbol table
    struct symbol_table *table = symbol_table_create(100);
    // build the symbol table
    build_symbol_table(buffer, table, file_size);

    // second pass
    // parse each line
    generate_binary(buffer, argv[2], table, file_size);

    // free pointer
    free(buffer);
    symbol_table_free(table);
    return EXIT_SUCCESS;
}
