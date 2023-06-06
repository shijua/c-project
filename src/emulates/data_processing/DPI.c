#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include "DPI.h"

void DPI(char* memory, struct Registers* registers, struct send_DPI divide){
    struct DPI_instruction instr; //instr parses the binary instruction to parameters usable for the function
    instr.sf = divide.sf;//coppy sf opc the same way it is in divide
    instr.opc = divide.opc;
    instr.opi = divide.opi == 0x2;
    instr.rd = divide.rd == 31 ? &(registers->ZR) : &(registers->general[divide.rd]);
    instr.operand = divide.operand;
    struct arithmetic_Operand opra; // arithmetic operation and wide move operation are parsed different so we construct them with different struct.
    struct wideMove_Operand oprw;
    if (instr.sf){
        instr.topBit = 63;
    }
    else{
        instr.topBit = 31;
    }
    if (instr.opi){
        opra.sh = get_bit (17 , 1 , instr.operand); //parsing arithmetic operator. details see in spec
        opra.imm12 = get_bit (16 , 12 , instr.operand);
        opra.rn = get_bit (4 , 5 , instr.operand) == 31 ? &(registers->ZR):&(registers->general [get_bit (4 , 5 , instr.operand)]);
        arithmetic (registers, instr , opra); // do arithmetic operation 
    }
    else{
        oprw.hw = get_bit (17 , 2 , instr.operand);//parsing wide move operator. details see in spec
        oprw.imm16 = get_bit (15 , 16 , instr.operand);
        wideMove (registers, instr , oprw); //do wide move operation
    }
    // clear left part if it is 32 bits
    if(!instr.sf) {
        *instr.rd = (*instr.rd) & (0xFFFFFFFF);
    }
}

void arithmetic (struct Registers* registers, struct DPI_instruction instr , struct arithmetic_Operand opr){
    if(opr.sh){
        opr.imm12 = opr.imm12 << 12; //if sh is 1 then right move by 12
    }
    long long addition = opr.imm12 + *opr.rn;
    long long subtraction = *opr.rn - opr.imm12;
    switch (instr.opc) //perform operation based on opc(operation code)
    {
    case 0:
        memcpy(instr.rd, &addition, 4 + instr.sf * 4);
        break;
    case 1:
        memcpy(instr.rd, &addition, 4 + instr.sf * 4);//addition with changing PSTATE
        registers->pstate.N = get_bit (instr.topBit , 1 , *instr.rd); //set N to the first bit of rd
        registers->pstate.Z = *instr.rd == 0; //set Z to 1 if all bits of rd are 0
        registers->pstate.C = hasCarryBorrow(opr.imm12, *opr.rn, instr.sf, 0); //set C to 1 if it addition has carry out
        registers->pstate.V = overflow(opr.imm12 , *opr.rn , instr.sf); //set V to 1 if there is overflow or underflow
        break;
    case 2:
        memcpy(instr.rd, &subtraction, 4 + instr.sf * 4);//subtraction
        break;
    case 3:
        memcpy(instr.rd, &subtraction, 4 + instr.sf * 4);//subtraction with changing PSTATE
        registers->pstate.N = get_bit (instr.topBit , 1 , *instr.rd);//set N to the first bit of rd
        registers->pstate.Z = *instr.rd == 0;//set Z to 1 if all bits of rd are 0
        registers->pstate.C = hasCarryBorrow(*opr.rn, opr.imm12, instr.sf, 1);//set C to 1 if it addition has borrow
        registers->pstate.V = overflow(opr.imm12 , *opr.rn , instr.sf);//set V to 1 if there is overflow or underflow
        break;
    default:
        printf("Error in OPC\n");
        assert(false);
    }

}

void wideMove (struct Registers* registers , struct DPI_instruction instr , struct wideMove_Operand opr){
    instr.operand = opr.imm16;
    // doing separately as imm16 is int
    int shift = opr.hw * 16;
    instr.operand <<= shift;
    // 64 1 bits
    long long lon1 = ~0LL;
    // use char as pointer I can modify long long byte by byte
    char* rd = (char*)instr.rd;
    switch (instr.opc)
    {
    case 0: // movn
        // set all 64 bits to 1
        memcpy(instr.rd, &lon1, 8);
        // reveerse the bits
        opr.imm16 = ~opr.imm16;
        // put the imm16 into the rd accoring to the shift
        memcpy(rd + (shift / 8), &opr.imm16 , 2);
        break;
    case 2: // movz
        // Move certain part of the register while all the rest bits are 0
        *instr.rd = instr.operand;
        break;
    case 3: // movk
        // Move certain part of the register while keep the rest of the bits
        memcpy(rd + (shift / 8), &opr.imm16 , 2);
        break;
    default:
        printf("Error: invalid instruction\n");
        assert(false);
    }
}
