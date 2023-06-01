#include <stdlib.h>
#include <stdio.h>
#include "emulates/Util.h"
#include "emulates/pipeline/input.h"
#include "emulates/pipeline/output.h"
// TODO max memory size
int memory[MAX_MEMORY];
struct Registers registers;
int main(int argc, char **argv) {
  readfile("../test/expected_results/branch/b1_exp.bin", memory);
  // initialise PC to 0
  registers.PC = 0;

  // fetch and decode steps
  int instruction = memory[registers.PC / 4];
  // decode(memory, &registers, instruction);
  while (instruction != 0 && registers.PC != -1)
  {
    decode(memory, &registers, instruction);
    registers.PC += 4;
    instruction = memory[registers.PC / 4];
  }
  
  printf("It runs!!!\n");
  // output the registers and memory
  output(&registers, memory);
  return EXIT_SUCCESS;	
}
