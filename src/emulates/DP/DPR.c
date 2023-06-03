#include "DPR.h"
#include "bitwiseShift.h"
#include <string.h>

void DPR(char* memory, struct Registers* registers, struct send_DPR divide){
    struct DPR_instruction instr;//instr parses the binary instruction to parameters usable for the function
    instr.sf = divide.sf;
    instr.opc = divide.opc;
    instr.rd = divide.rd == 31 ? &(registers->ZR):&(registers->general [divide.rd]); //point rd , rm , and rn to the correct register
    instr.rm = divide.rm == 31 ? &(registers->ZR):&(registers->general [divide.rm]); //when they are 11111 , point to ZR, otherwise general register
    instr.rn = divide.rn == 31 ? &(registers->ZR):&(registers->general [divide.rn]);
    instr.M = divide.M;
    instr.opr = divide.opr;
    instr.operand = divide.operand;
    int shift = get_bit (2 , 2 , instr.opr);
    long long OP2;
    switch (shift) // perform shift based on the mid two bits of opr
    {
    case 0:
        OP2 = logicalShiftLeft(*instr.rm , instr.operand , instr.sf); 
        break;
    case 1:
        OP2 = logicalShiftRight(*instr.rm , instr.operand , instr.sf); 
        break;
    case 2:
        OP2 = arithmeticShiftRight(*instr.rm , instr.operand , instr.sf); 
        break;
    case 3:
        OP2 = rotateRight(*instr.rm , instr.operand , instr.sf); 
        break;
    default:
        break;
    }
    

    if(!instr.sf){
        instr.topBit = 31;

    }
    else{
        instr.topBit = 63;

    }

    if(instr.opr>=8){
        //check if opr is 1xxx or 0xxx, if it is 1xxx then we perform arithmetic operation or Multiplication
        //otherwise perform logic operation.
        if(instr.M){
            long long * ra = get_bit (4 , 5 , instr.operand) == 31 ? &(registers->ZR):&(registers->general [get_bit (4 , 5 , instr.operand)]);
            if(get_bit (5 , 1 , instr.operand)){
                *instr.rd = *ra - *instr.rn * (*instr.rm);
            }
            else{
                *instr.rd = *ra + *instr.rn * (*instr.rm);
            }
            if(!instr.sf) {
                // if 32 bit register
                *instr.rd = (*instr.rd) & (0xFFFFFFFF);
            }
        }
        else{
            Arithmetic_Operation (instr , OP2 , registers);
        }
    }
    else{
        if(get_bit (0 , 1 , instr.opr)){
            OP2 = ~OP2; // perfrom negation if N 
        }
        Logical_Operation (instr , OP2 , registers); 
    }

}

void Logical_Operation(struct DPR_instruction instr , long long OP2 , struct Registers* registers){
    long long res;
    switch (instr.opc)
        {
        case 0:
            res = *instr.rn & OP2;
            memcpy(instr.rd, &res, 4 + 4*instr.sf);
            break;
        case 1:
            res = *instr.rn | OP2;
            memcpy(instr.rd, &res, 4 + 4*instr.sf);
            break;
        case 2:
            res = *instr.rn ^ OP2;
            memcpy(instr.rd, &res, 4 + 4*instr.sf);
            break;
        case 3:
            res = *instr.rn & OP2;
            memcpy(instr.rd, &res, 4 + 4*instr.sf);
            registers->pstate.N = get_bit (instr.topBit , 1 , *instr.rd);
            registers->pstate.Z = *instr.rd == 0;
            registers->pstate.C = 0;
            registers->pstate.V = 0;
            break;
        default:
            break;
        }
}


void  Arithmetic_Operation (struct DPR_instruction instr , long long OP2, struct Registers* registers){
    long long addition = OP2 + *instr.rn;
    long long subtraction = *instr.rn - OP2;
    switch (instr.opc) //perform operation based on opc(operation code)
    {
    case 0:
        if (!instr.sf)
            memcpy(instr.rd, &addition, 4);
        else
            *instr.rd = addition; //addition
        break;
    case 1:
        if (!instr.sf)
            memcpy(instr.rd, &addition, 4);
        else
            *instr.rd = addition;  //addition with changing PSTATE

        registers->pstate.N = get_bitl (instr.topBit , 1 , *instr.rd); //set N to the first bit of rd
        registers->pstate.Z = *instr.rd == 0; //set Z to 1 if all bits of rd are 0
        registers->pstate.C = hasCarryOut(OP2, *instr.rn); //set C to 1 if it addition has carry out
        registers->pstate.V = overflow(OP2 , *instr.rn , instr.sf); //set V to 1 if there is overflow or underflow
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
        registers->pstate.C = hasBorrow(OP2, *instr.rn);//set C to 1 if it addition has borrow
        registers->pstate.V = overflow(OP2 , *instr.rn , instr.sf);//set V to 1 if there is overflow or underflow
        break;
    default:
        break;
    }

}
