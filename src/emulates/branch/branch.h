#include "../Util.h"
#include "../inputformat.h"
struct literal{
    int sf;
    int simM26;
    int Xn;
    int SimM19;
    int cond;
};
void process(struct send_branch pre, struct literal *lit);
unsigned int getBitRange(int num, int x, int y);
void printBinary(unsigned int num);
void Branch(int* memory, struct Registers *r, struct send_branch pre);
bool conditional_branch(struct Registers * r, unsigned int condition);
