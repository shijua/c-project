#include <stdlib.h>
#include <stdio.h>
#include "emulates/Util.h"
#include "emulates/pipeline/input.h"
#include "emulates/pipeline/output.h"
int memory[MAX_MEMORY];
struct Registers registers;
int main(int argc, char **argv) {
  readfile("../test/expected_results/branch/b1_exp.bin", memory);
  registers.PC = 0;
  int instruction = memory[registers.PC / 4];
  decode(memory, &registers, instruction);
  printf("It runs!!!\n");
  return EXIT_SUCCESS;	
}
