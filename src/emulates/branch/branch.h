#include "../Util.h"
#include "../inputformat.h"
struct literal{
    int sf;
    int simM26;
    int Xn;
    int SimM19;
    int cond;
};
extern void process(struct send_branch pre, struct literal* lit);
extern void Branch(char* memory, struct Registers* r, struct send_branch pre);
extern bool conditional_branch(struct Registers* r, unsigned int condition);
