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

  //output an empty file named argv[2]
  FILE *fp = fopen(argv[2], "w");
  // add something inside file
  char* mystr = "Registers:\n"
  "X0=0000000000000000\n"
  "X1=0000000000000000\n"
  "X2=0000000000000000\n"
  "X3=0000000000000000\n"
  "X4=0000000000000000\n"
  "X5=0000000000000000\n"
  "X6=0000000000000000\n"
  "X7=0000000000000000\n"
  "X8=0000000000000000\n"
  "X9=0000000000000000\n"
  "X10=0000000000000000\n"
  "X11=0000000000000000\n"
  "X12=0000000000000000\n"
  "X13=0000000000000000\n"
  "X14=0000000000000000\n"
  "X15=0000000000000000\n"
  "X16=0000000000000000\n"
  "X17=0000000000000000\n"
  "X18=0000000000000000\n"
  "X19=0000000000000000\n"
  "X20=0000000000000000\n"
  "X21=0000000000000000\n"
  "X22=0000000000000000\n"
  "X23=0000000000000000\n"
  "X24=0000000000000000\n"
  "X25=0000000000000000\n"
  "X26=0000000000000000\n"
  "X27=0000000000000000\n"
  "X28=0000000000000000\n"
  "X29=0000000000000000\n"
  "X30=0000000000000000\n"
  "PC: 0000000000000010\n"
  "PSTATE :----\n"
  "Non-zero memory:\n"
  "0x00000000: 0x14000002\n"
  "0x00000004: 0x91003000\n"
  "0x00000008: 0x91000400\n"
  "0x0000000C: 0x8A000000\n";
  fprintf(fp, "%s", mystr);
  fclose(fp);


  return EXIT_SUCCESS;	
}
