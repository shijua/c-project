#include <string.h>
#include <stdbool.h>
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
        opra.max_value = (long long) 9223372036854775807; //this is useful for detecting overflow and underflow for 64-bit arithmetic
        opra.min_value = (long long)-9223372036854775807;
    }
    else{
        instr.topBit = 31;
        opra.max_value = 2147483647 ; //this is useful for detecting overflow and underflow for 32-bit arithmetic
        opra.min_value = -2147483648 ;
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

}

void arithmetic (struct Registers* registers, struct DPI_instruction instr , struct arithmetic_Operand opr){
    if(opr.sh){
        opr.imm12 = opr.imm12<<12; //if sh is 1 then right move by 12
    }
    long long addition = opr.imm12 + *opr.rn;
    long long subtraction = *opr.rn - opr.imm12;
    switch (instr.opc) //perform operation based on opc(operation code)
    {
    case 0:
        if (!instr.sf)
            memcpy(instr.rd, &addition, 4);
        else
            *instr.rd = addition;  //addition
        break;
    case 1:
        if (!instr.sf)
            memcpy(instr.rd, &addition, 4);
        else
            *instr.rd = addition;//addition with changing PSTATE
        registers->pstate.N = get_bit (instr.topBit , 1 , *instr.rd); //set N to the first bit of rd
        registers->pstate.Z = *instr.rd == 0; //set Z to 1 if all bits of rd are 0
        registers->pstate.C = hasCarryOut(opr.imm12, *opr.rn); //set C to 1 if it addition has carry out
        registers->pstate.V = (opr.imm12 > opr.max_value - *opr.rn || opr.imm12 < opr.min_value + *opr.rn); //set V to 1 if there is overflow or underflow
        break;
    case 2:
        if (!instr.sf)
            memcpy(instr.rd, &subtraction, 4);
        else
            *instr.rd = subtraction; //subtraction
        break;
    case 3:
        if (!instr.sf)
            memcpy(instr.rd, &subtraction, 4);
        else
            *instr.rd = subtraction; //subtraction with changing PSTATE
        registers->pstate.N = get_bit (instr.topBit , 1 , *instr.rd);//set N to the first bit of rd
        registers->pstate.Z = *instr.rd == 0;//set Z to 1 if all bits of rd are 0
        registers->pstate.C = hasBorrow(opr.imm12, *opr.rn);//set C to 1 if it addition has borrow
        registers->pstate.V = (*opr.rn > opr.max_value + opr.imm12 || *opr.rn < opr.min_value - opr.imm12);//set V to 1 if there is overflow or underflow
        break;
    default:
        break;
    }

}

void wideMove (struct Registers* registers , struct DPI_instruction instr , struct wideMove_Operand opr){
    instr.operand = opr.imm16<<(opr.hw * 16);
    switch (instr.opc)
    {
    case 1:
        *instr.rd = instr.operand; // Move certain part of the register while all the rest bits are 0
        break;
    case 0:
        *instr.rd = -1LL; // Move certain part of the register while all the rest bits are 1
        copyBits(*instr.rd , instr.rd , opr.hw*16 , (opr.hw+1)*16);
        break;
    case 2:
        copyBits(*instr.rd , instr.rd , opr.hw*16 , (opr.hw+1)*16);// Move certain part of the register while keep the rest of the bits
        break;
    default:
        break;
    }
}


void copyBits(long long source, long long* destination, int startBit, int endBit) {
    // creat a mask, this will set the bits that we want to copy to 1 and all the rest to 0
    // it will be used to set the bits in the destination that we care about to 0
    long long mask = ((1LL << (endBit - startBit + 1)) - 1) << startBit;
    
    // erase the bits that we want to move on destination
    *destination &= ~mask;
    
    // move the bits to destination
    *destination |= (source & mask);
}


