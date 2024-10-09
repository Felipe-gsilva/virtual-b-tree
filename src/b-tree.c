#include "b-tree.h"

page *create_new_tree(page *page) {
    return new_page(0, 0, 0);
}

bool remove_page() {
    return false;
}

void driver(){}

u16 search(u16 rrn, key key, u16 found_rrn, u16 found_pos);

void driver();

bool insert_page();

bool split();

bool promote();

page *redistribute();

FILE *create_tree_file(char *file_name);

void print_tree(b_tree *tree);
