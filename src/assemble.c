#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "assembles/parser/input.h"
#include "assembles/symboltable.h"
#include "assembles/parser/parser.h"
int main(int argc, char **argv) {
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


    // write test case for parse
    // char test[] = "nop";
    // unsigned int *instruction = malloc(sizeof(int));
    // parse(test, 0, instruction, NULL);

    // parse("add x0, x1, #2", 0, NULL, NULL);
    // parse("add x0, x1, x2, lsl #2", 0, NULL, NULL);
    // // for and
    // parse("and x0, x1, x2", 0, NULL, NULL);
    // // with shift
    // parse("and x0, x1, x2, lsl #2", 0, NULL, NULL);
    // // for b
    // parse("b #2", 0, NULL, NULL);
    // // for b.cond
    // parse("b.eq #2", 0, NULL, NULL);
    // // for str
    // parse("str x0, [x1]", 0, NULL, NULL);
    // // for ldr
    // parse("ldr x0, [x1]", 0, NULL, NULL);
    // // for ldr with offset
    // parse("ldr x0, [x1, #2]", 0, NULL, NULL);
    // // for ldr with register offset
    // parse("ldr x0, [x1, x2, lsl #2]", 0, NULL, NULL);
    // // for ldr with pre-index
    // parse("ldr x0, [x1, #2]!", 0, NULL, NULL);
    // // for ldr with post-index
    // parse("ldr x0, [x1], #2", 0, NULL, NULL);
    // // for ldr with unsigned offset
    // parse("ldr x0, [x1, #2]", 0, NULL, NULL);
    // // for ldr with literal
    // parse("ldr x0, =2", 0, NULL, NULL);
    // // for nop
    // parse("nop", 0, NULL, NULL);
    // // for .int
    // parse(".int 2", 0, NULL, NULL);




    return EXIT_SUCCESS;
}
