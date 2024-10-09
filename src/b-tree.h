#ifndef _btree
#define _btree

#include "page.h"

#define ERROR -1
#define NOT_FOUND 0
#define FOUND 1
#define PROMOTION 2
#define NO_PROMOTION 3

typedef struct b_tree b_tree;

typedef struct b_tree_header b_tree_header;

struct b_tree_header {
    u16 root_rrn;
    u16 page_size;
};

struct b_tree {
    b_tree_header *bh;
    io_buf *io;
};


b_tree *create_new_tree();

b_tree *alloc_tree_buf();

void clear_tree_buf(b_tree *b);

void driver();

u16 search(b_tree *b, u16 rrn, key key, u16 found_rrn, u16 found_pos, page *return_page);

bool remove_page(b_tree *b, page *page);

bool insert_page(b_tree *b, page *page);

page* split(page *page);

bool promote();

FILE *create_tree_file(char *file_name);

page *load_page(b_tree *b, u16 rrn);

u16 search_key(page *page, key key);

#endif
