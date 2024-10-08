#ifndef _btree
#define _btree

#include "page.h"

#define ERROR -1
#define NOT_FOUND 0
#define FOUND 1

#define PROMOTION 2
#define NO_PROMOTION 3


typedef struct b_tree b_tree;
struct b_tree {
    // TODO save b-tree root?
    // armazenará no seu cabeçalho o RNN da raiz seguida das demais chaves.
};

page *create_new_tree(page *page);

u16 search(u16 rrn, key key, u16 found_rrn, u16 found_pos);

void driver();

bool remove_page();

bool insert_page();

bool split();

bool promote();

FILE *create_tree_file(char *file_name);

#endif
