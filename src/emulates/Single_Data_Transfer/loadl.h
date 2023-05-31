//
// Created by LIHAOMIN on 2023/5/30.
//
#include "../../emulate.h"

struct loadliteral { // loadliteral parameter
    bool sf;
    int simm19;
    int rt;
};
void LoadLiteral(int* memory, struct Registers regs, struct loadliteral l);