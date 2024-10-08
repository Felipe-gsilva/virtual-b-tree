#include "stack.h"

stack *alloc_stack() {
    stack* root = malloc(sizeof(stack));
    root->next = NULL;
    return root;
}

void clear_stack(stack *stack) {
    if(!stack) {
        puts("!!Error: NULL stack pointer");
        return;
    }   

}

void print_stack(stack *stack) {

    if(!stack) {
        puts("!!Error: NULL stack pointer");
        return;
    }   

}

void push(stack *stack, page *page) {
//    page *aux;
//    if(!stack) {
//        puts("!!Error: NULL stack pointer");
//        return;
//    }   
//    aux = new_page(1,0,0);
//    *stack = aux;
    return;
}

page *pop(stack *stack, page *page) {
    if(!stack) {
        puts("!!Error: NULL stack pointer");
        return stack->next;
    }   

}
