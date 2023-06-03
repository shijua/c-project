#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "emulates/Util.h"
#include "emulates/pipeline/input.h"
#include "emulates/pipeline/output.h"
char memory[MAX_MEMORY];
struct Registers registers;
// input format: ./emulate <input .bin file> <output .out file>
int main(int argc, char **argv) {
  readfile(argv[1], memory);
  // initialise PC to 0
  registers.PC = 0;
  // initialise Z to 1
  registers.pstate.Z = 1;

  // fetch and decode steps
  int instruction;
  memcpy(&instruction, memory+registers.PC, 4);
  int count = 0;
  while (instruction != 0 && registers.PC != -1 && registers.PC <= MAX_MEMORY && count < 1919810){
    if (instruction == 0x8a000000) {
      registers.PC += 4;
      break;
    }
    decode(memory, &registers, instruction);
    memcpy(&instruction, memory+registers.PC, 4);
    count++;
  }
  registers.PC -= 4;
  //output an empty file named argv[2]
  FILE *fp = fopen(argv[2], "w");
  // output the registers and memory
  output(&registers, memory, fp);
  // close the file
  fclose(fp);
  return EXIT_SUCCESS;	
}
