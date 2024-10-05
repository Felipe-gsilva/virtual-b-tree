#ifndef _STACK
#define _STACK

#include "page.h"

#define P 50

typedef struct stack stack;

struct stack {
    page page[P];
};

#endif
