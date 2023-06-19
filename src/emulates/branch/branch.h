#include <stdint.h>
#include "../Util.h"
#include "../inputformat.h"
struct literal{
    uint8_t sf;
    int32_t simM26;
    uint8_t Xn;
    int32_t SimM19;
    int32_t cond;
};
extern void process(struct send_branch pre, struct literal* lit);
extern void Branch(char* memory, struct Registers* r, struct send_branch pre);
extern bool conditional_branch(struct Registers* r, uint32_t condition);
