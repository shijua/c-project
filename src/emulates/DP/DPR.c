#include "DPR.h"
#include "bitwiseShift.h"

void DPR(char* memory, struct Registers* registers, struct send_DPR divide){
    struct DPR_instruction instr;
    instr.sf = divide.sf;
    instr.opc = divide.opc;
    instr.rd = divide.rd == 31 ? &(registers->ZR):&(registers->general [divide.rd]);
    instr.rm = divide.rm == 31 ? &(registers->ZR):&(registers->general [divide.rm]);
    instr.rn = divide.rn == 31 ? &(registers->ZR):&(registers->general [divide.rn]);
    instr.M = divide.M;
    instr.opr = divide.opr;
    instr.operand = divide.operand;
    int shift = get_bit (2 , 2 , instr.opr);
    long long OP2;
    switch (shift)
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
        OP2 = (int) OP2;
    }

    if(instr.opr>=8){
        
        
        if(instr.M){
            long long * ra = get_bit (4 , 5 , instr.operand) == 31 ? &(registers->ZR):&(registers->general [get_bit (4 , 5 , instr.operand)]);
            if(get_bit (5 , 1 , instr.operand)){
                *instr.rd = *ra - *instr.rn * (*instr.rm);
            }
            else{
                *instr.rd = *ra + *instr.rn * (*instr.rm);
            }
        }
        else{
            arithAndLogic (instr , OP2 , registers);
        }
    }
    else{
        if(get_bit (0 , 1 , instr.opr)){
            OP2 = ~OP2;
        }
        arithAndLogic (instr , OP2 , registers); 
    }

}

void arithAndLogic(struct DPR_instruction instr , long long OP2 , struct Registers* registers){
    switch (instr.opc)
        {
        case 0:
            *instr.rd = *instr.rn & OP2;
            break;
        case 1:
            *instr.rd = *instr.rn | OP2;
            break;
        case 2:
            *instr.rd = *instr.rn ^ OP2;
            break;
        case 3:
            *instr.rd = *instr.rn & OP2;
            registers->pstate.N = get_bit (4 , 1 , *instr.rd);
            registers->pstate.Z = *instr.rd == 0;
            registers->pstate.C = 0;
            registers->pstate.V = 0;
            break;
        default:
            break;
        }
}

