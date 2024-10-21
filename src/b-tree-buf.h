#ifndef _B_TREE_BUF_H
#define _B_TREE_BUF_H

#include "defines.h"

#define ERROR -1
#define NOT_FOUND -1
#define FOUND 1
#define PROMOTION 2
#define NO_PROMOTION 3

b_tree_buf *alloc_tree_buf();

void build_tree(b_tree_buf *b, io_buf *data, int n);

void populate_key(key *k,data_record *d, u16 rrn);

void insert(b_tree_buf *b,io_buf *data,  data_record *d, u16 rrn);

int insert_key(b_tree_buf *b, page *p, key k, key *promo_key, page *r_child);

void split(page *p, key k, page *r_child, key *promo_key, page *new_page, int pos);

void insert_in_page(page *p, key k, page *r_child, int pos);

void promote();

void create_index_file(io_buf *io, char *file_name);

void create_list_file(FILE *fp , char *file_name);

void clear_tree_buf(b_tree_buf*b);

page *search(b_tree_buf *b, const char *s);

u16 search_key(b_tree_buf *b, page *p, key key, u16 *found_pos, page *return_page);

int search_in_page(page *page, key key, int *return_pos);

int remove_key( b_tree_buf*b, page *page);

void b_remove(b_tree_buf *b, io_buf *data, char *s);

void print_page(page *page);

page *load_page(io_buf *io, queue *q, u16 rrn);

void populate_tree_header(index_header_record *bh, char *file_name);

void read_index_header(io_buf *io);

void write_index_header(io_buf *io);

void write_index_record(io_buf *io, page *p);

page *alloc_page();

page *new_page(u16 rrn);

void clear_page(page *page);

#endif
