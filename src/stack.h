#ifndef _STACK
#define _STACK

#include "page.h"

#define P 50

typedef struct stack stack;

struct stack {
    page *next;
    page page;
    u8 counter;
};


stack *alloc_stack();

void clear_stack(stack *stack);

void print_stack(stack *stack);

void push(stack *stack, page *page);

page *pop(stack *stack, page *page);

#endif
