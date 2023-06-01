#include <string.h>
#include <stdbool.h>
#include "DPI.h"

void DPI(int* memory, struct Registers* registers, struct send_DPI divide){
    struct instruction instr;
    instr.sf = divide.sf;
    instr.opc = divide.opc;
    instr.opi = divide.opi == 0x2;
    instr.rd = divide.rd == 0b11111 ? registers->ZR: registers->general + divide.rd;
    instr.operand = divide.operand;
    struct arithmetic_Operand opra;
    struct wideMove_Operand oprw;
    if (instr.sf){
        opra.max_value = 9223372036854775807;
        opra.min_value = -9223372036854775808;
    }
    else{
        opra.max_value = 2147483647 ;
        opra.min_value = -2147483648 ;
    }
    if (instr.opi){
        
        opra.sh = get_bit (17 , 1 , instr.operand);
        opra.imm12 = get_bit (16 , 12 , instr.operand);
        opra.rn = get_bit (4 , 5 , instr.operand) == 0b11111 ? registers->ZR:registers->general + get_bit (4 , 5 , instr.operand);
        arithmetic (registers, instr , opra);
    }
    else{
        
        oprw.hw = get_bit (17 , 2 , instr.operand);
        oprw.imm16 = get_bit (15 , 16 , instr.operand);
        wideMove (registers, instr , oprw);
    }

}

void arithmetic (struct Registers* registers, struct instruction instr , struct arithmetic_Operand opr){
    if(opr.sh){
        opr.imm12 = opr.imm12<<12;
    }
    switch (instr.opc)
    {
    case 0:
        *instr.rd = opr.imm12 + *opr.rn; 
        break;
    case 1:
        *instr.rd = opr.imm12 + *opr.rn; 
        registers->pstate.N = get_bit (4 , 1 , *instr.rd);
        registers->pstate.Z = *instr.rd == 0;
        registers->pstate.C = hasCarryOut(opr.imm12, *opr.rn);
        registers->pstate.V = (opr.imm12 > opr.max_value - *opr.rn || opr.imm12 < opr.max_value + *opr.rn);
        break;
    case 2:
        *instr.rd = opr.imm12 - *opr.rn; 
        break;
    case 3:
        *instr.rd = opr.imm12 - *opr.rn; 
        registers->pstate.N = get_bit (4 , 1 , *instr.rd);
        registers->pstate.Z = *instr.rd == 0;
        registers->pstate.C = hasBorrow(opr.imm12, *opr.rn);
        registers->pstate.V = (opr.imm12 > opr.max_value + *opr.rn || opr.imm12 < opr.max_value - *opr.rn);
        break;
    default:
        break;
    }

}

void wideMove (struct Registers* registers , struct instruction instr , struct wideMove_Operand opr){
    instr.operand = opr.imm16<<(opr.hw * 16);
    switch (instr.opc)
    {
    case 1:
        *instr.rd = instr.operand;
        break;
    case 0:
        *instr.rd = -1LL;
        copyBits(*instr.rd , instr.rd , opr.hw*16 , (opr.hw+1)*16);
        break;
    case 2:
        copyBits(*instr.rd , instr.rd , opr.hw*16 , (opr.hw+1)*16);
        break;
    default:
        break;
    }
}

int hasCarryOut(int a, int b) {
    int carry = a & b;  
    return carry != 0;  
}

int hasBorrow(int a, int b) {
    int borrow = (~a) & b;  
    return borrow != 0;  
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
