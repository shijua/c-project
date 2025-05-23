#include "DPR.h"
#include "bitwiseShift.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

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
    uint8_t shift = get_bit (2 , 2 , instr.opr);
    int64_t OP2;
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
        printf("Error in OP2\n");
        assert(false);
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
            int64_t * ra;
            if(get_bitl (4 , 5 , instr.operand) == 31) {
                ra = &(registers->ZR);
            }
            else{
                ra = &(registers->general [get_bitl (4 , 5 , instr.operand)]);
            }
            
            if(get_bit (5 , 1 , instr.operand)){
                *instr.rd = *ra - *instr.rn * (*instr.rm);
            }
            else{
                *instr.rd = *ra + *instr.rn * (*instr.rm);
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
    // clear left part if it is 32 bits
    if(!instr.sf) {
        *instr.rd = (*instr.rd) & (0xFFFFFFFF);
    }
}

void Logical_Operation(struct DPR_instruction instr , int64_t OP2 , struct Registers* registers){
    int64_t res;
    switch (instr.opc)
        {
        case 0:
            res = *instr.rn & OP2;
            *instr.rd = 0;
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
            registers->pstate.N = get_bitl (instr.topBit , 1 , *instr.rd);
            registers->pstate.Z = *instr.rd == 0;
            registers->pstate.C = 0;
            registers->pstate.V = 0;
            break;
        default:
            printf("Error in OPC\n");
            assert(false);
        }
}


void  Arithmetic_Operation (struct DPR_instruction instr , int64_t OP2, struct Registers* registers){
    int64_t addition = OP2 + *instr.rn;
    int64_t subtraction = *instr.rn - OP2;
    switch (instr.opc) //perform operation based on opc(operation code)
    {
    case 0:
        memcpy(instr.rd, &addition, 4 + 4*instr.sf);//addition
        break;
    case 1:
        memcpy(instr.rd, &addition, 4 + 4*instr.sf);//addition with changing PSTATE
        registers->pstate.N = get_bitl (instr.topBit , 1 , *instr.rd); //set N to the first bit of rd
        registers->pstate.Z = *instr.rd == 0; //set Z to 1 if all bits of rd are 0
        registers->pstate.C = hasCarryBorrow(*instr.rn, OP2, instr.sf, 0); //set C to 1 if it addition has carry out
        registers->pstate.V = overflow(*instr.rn, OP2, instr.sf); //set V to 1 if there is overflow or underflow
        break;
    case 2:
        memcpy(instr.rd, &subtraction, 4 + 4*instr.sf);//subtraction
        break;
    case 3:
        memcpy(instr.rd, &subtraction, 4 + 4*instr.sf);//subtraction with changing PSTATE
        registers->pstate.N = get_bitl (instr.topBit , 1 , *instr.rd);//set N to the first bit of rd
        registers->pstate.Z = *instr.rd == 0;//set Z to 1 if all bits of rd are 0
        registers->pstate.C = hasCarryBorrow(*instr.rn, OP2, instr.sf, 1);//set C to 1 if it addition has borrow
        registers->pstate.V = overflow(*instr.rn, OP2, instr.sf); //set V to 1 if there is overflow or underflow
        break;
    default:
        printf("Error in OPC\n");
        assert(false);
    }

}
