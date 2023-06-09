#include "data_processing.h"
#include "../Util.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void copy_arithm_opc (unsigned int *instr , char* opcode){
    if(!strcmp (opcode, "add")){
        copy_bit (instr , 0 , 29 , 30);
    }
    if(!strcmp (opcode, "adds")){
        copy_bit (instr , 1 , 29 , 30);
    }
    if(!strcmp (opcode, "sub")){
        copy_bit (instr , 2 , 29 , 30);
    }
    if(!strcmp (opcode, "subs")){
        copy_bit (instr , 3 , 29 , 30);
    }
}

void copy_opr (unsigned int *instr , char* shift , bool is_arith , bool N){
    // TODO string cost lots of time
    if (!strcmp(substring(shift , 0 , 3) , "lsl")){
        copy_bit (instr , 8*is_arith + N , 21 , 24);//shift type : lsl
    }
    if (!strcmp(substring(shift , 0 , 3) , "lsr"))
    {
        copy_bit (instr , 8*is_arith + N + 2 , 21 , 24);//shift type : lsr
    }
    if (!strcmp(substring(shift , 0 , 3) , "asr"))
    {
        copy_bit (instr , 8*is_arith + N + 4 , 21 , 24);//shift type : asr
    }
    if (!strcmp(substring(shift , 0 , 3) , "ror") && !is_arith)
    {
        copy_bit (instr , 8*is_arith + N + 6 , 21 , 24);//shift type : ror
    }
}

extern void tokenise_add_sub_immediate (unsigned int* instr , struct add_sub_immediate divide){
    copy_arithm_opc (instr , divide.opcode); //opc
    copy_bit (instr , (check_bit (divide.rd)) , 31 , 31);//sf
    copy_bit (instr , 4 , 26 , 28);//op0
    copy_bit (instr , 2 , 23 , 25);//opi
    copy_bit (instr , register_to_bin(divide.rd) , 0 , 4);//rd
    if (divide.shift == NULL || to_int (divide.shift + 4) == 0)
        copy_bit (instr , 0 , 22 , 22) ;
    else
        copy_bit (instr , 1 , 22 , 22);//sh
    //divide.imm + 3 is for eliminating the prefix "#0x" of imm
    //strtol is turns a string representing hex to a int.
    copy_bit (instr , to_int(divide.imm) , 10 , 21); //imm12
    copy_bit (instr , register_to_bin(divide.rn) , 5 , 9); //rn

}

extern void tokenise_add_sub_register (unsigned int* instr , struct add_sub_register divide){
    copy_bit (instr , register_to_bin(divide.rd) , 0 , 4);//rd
    copy_bit (instr , register_to_bin(divide.rn) , 5 , 9); //rn
    copy_bit (instr , register_to_bin(divide.rm) , 16 , 20); //rm
    copy_bit (instr , (check_bit (divide.rd)) , 31 , 31);//sf
    copy_arithm_opc (instr , divide.opcode); // opc
    copy_bit (instr , 5 , 25 , 28);//M and bit 25 to 27 are constant, being 0101 which is equal to 5
    if(divide.shift != NULL){
        copy_opr (instr , divide.shift , 1 , 0); //opr
        
        copy_bit(instr , atoi(divide.shift + 5) , 10 , 15);//operand
    }
    else{
        copy_bit (instr , 8, 21 , 24);//opr
        copy_bit(instr , 0 , 10 , 15);//operand
    }

}

extern void tokenise_logical (unsigned int* instr , struct logical divide){
    copy_bit (instr , register_to_bin(divide.rd) , 0 , 4);//rd
    copy_bit (instr , register_to_bin(divide.rn) , 5 , 9); //rn
    copy_bit (instr , register_to_bin(divide.rm) , 16 , 20); //rm
    copy_bit (instr , (check_bit (divide.rd)) , 31 , 31);//sf
    copy_bit (instr , 5 , 25 , 28);//M and bit 25 to 27 are constant, being 0101 which is equal to 5
    bool N = 0;
    if(!strcmp (divide.opcode, "and") || !strcmp (divide.opcode, "bic")){
        copy_bit (instr , 0 , 29 , 30);
        if(!strcmp (divide.opcode, "bic")) N =1;
    }
    if(!strcmp (divide.opcode, "orr") || !strcmp (divide.opcode, "orn")){
        copy_bit (instr , 1 , 29 , 30);
        if(!strcmp (divide.opcode, "orn")) N =1;
    }
    if(!strcmp (divide.opcode, "eon") || !strcmp (divide.opcode, "eor")){
        copy_bit (instr , 2 , 29 , 30);
        if(!strcmp (divide.opcode, "eor")) N =1;
    }
    if(!strcmp (divide.opcode, "ands") || !strcmp (divide.opcode, "bics")){
        copy_bit (instr , 3 , 29 , 30);
        if(!strcmp (divide.opcode, "bics")) N =1;
    }

    if(divide.shift != NULL){
        copy_opr (instr , divide.shift , 0 , N); //opr
        copy_bit(instr , atoi(divide.shift + 5) , 10 , 15);//operand
    }
    else{
        copy_bit (instr , N, 21 , 24);//opr
        copy_bit(instr , 0 , 10 , 15);//operand
    }
}

extern void tokenise_move_wide (unsigned int* instr , struct move_wide divide){
    if(!strcmp (divide.opcode, "movz")){  //opc
        copy_bit (instr , 0 , 29 , 30);
    }
    if(!strcmp (divide.opcode, "movn")){
        copy_bit (instr , 2 , 29 , 30);
    }
    if(!strcmp (divide.opcode, "movk")){
        copy_bit (instr , 3 , 29 , 30);
    }
    copy_bit (instr , 2 , 23 , 25);//opi
    copy_bit (instr , (check_bit (divide.rd)) , 31 , 31);//sf
    copy_bit (instr , 4 , 26 , 28);//op0
    copy_bit (instr , 2 , 23 , 25);//opi
    copy_bit (instr , register_to_bin(divide.rd) , 0 , 4);//rd
    //divide.imm + 3 is for eliminating the prefix "#0x" of imm
    //strtol is turns a string representing hex to a int.
    copy_bit (instr , to_int(divide.imm), 5 , 20); //imm12
    if(divide.shift != NULL){
        assert (atoi(divide.shift + 5)%16 == 0);
        copy_bit(instr , atoi(divide.shift + 5)/16 , 21 , 22);
    }
    else{
        copy_bit(instr , 0 , 21 , 22);
    }
}

void tokenise_multiply(unsigned int *instr, struct multiply divide){
    copy_bit (instr , register_to_bin(divide.rd) , 0 , 4);//rd
    copy_bit (instr , register_to_bin(divide.rn) , 5 , 9); //rn
    copy_bit (instr , register_to_bin(divide.rm) , 16 , 20); //rm
    copy_bit (instr , register_to_bin(divide.ra) , 10 , 14); //ra
    copy_bit (instr , (check_bit (divide.rd)) , 31 , 31);//sf
    copy_bit (instr ,  216, 21 , 30);//bits from 21 to 30 are constant
    if(!strcmp (divide.opcode, "madd")){
        copy_bit(instr , 0 , 15 , 15);
    }
    else{
        copy_bit(instr , 1 , 15 , 15);
    }
}

// int main(){
    
// }



