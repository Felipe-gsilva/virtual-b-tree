#ifndef _btree
#define _btree

#include "defines.h"

#define ERROR -1
#define NOT_FOUND -1
#define FOUND 1
#define PROMOTION 2
#define NO_PROMOTION 3

b_tree_buf*alloc_tree_buf();

void create_new_tree(b_tree_buf*b, io_buf *data, io_buf *index, int n);

void create_index_file(io_buf *io, char *file_name);

void clear_tree_buf(b_tree_buf*b);

void driver();

u16 search(b_tree_buf*b, io_buf *io, page *p, key key, u16 *found_rrn, u16 *found_pos, page *return_page);

u16 search_key(page *page, key key, int *return_pos);

int remove_key(io_buf *io, b_tree_buf*b, page *page);

u16 insert_key(b_tree_buf *b, io_buf *io, page *p, key key, page *return_page);

page* split(page *page);

int promote();

page *load_page(io_buf *io, queue *q, u16 rrn);

void print_page(page *page);

void populate_tree_header(index_header_record *b);


void read_index_header(io_buf *io);

void write_index_header(io_buf *io);

void write_index_record(io_buf *io, page *p);

// struct key {
//   u16 data_register_rrn;
//   char id[TAMANHO_PLACA];
// };
// 
// struct page{
//   u16 rrn;
//   key keys[ORDER-1];            
//   u16 children[ORDER];
//   u8 child_number;
//   u16 father;
// };


#endif
