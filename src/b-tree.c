#include "b-tree.h"
#include "io-buf.h"
#include "queue.h"
#include "page.h"

b_tree_buf *alloc_tree_buf() {
  b_tree_buf *b = malloc(sizeof(b_tree_buf));
  b->bh = malloc(sizeof(index_header_record));
  b->io = alloc_io_buf();
  b->q = alloc_queue();
  if(b) {
    if(DEBUG)
      puts("@Allocated b_tree_buf_BUFFER");
    return b;
  }
  puts("!!Could not allocate b_tree_buf_BUFFER");
  return NULL;
}

void clear_tree_buf(b_tree_buf *b) {
  if(b) {
    clear_io_buf(b->io);
    clear_queue(b->q);
    if(b->bh){
      free(b->bh);
      b->bh = NULL;
    }
    if(b->root) {
      clear_page(b->root);
      b->root = NULL;
    }
    free(b);
    b = NULL;
  }
  if(DEBUG)
    puts("@b_tree_buf_BUFFER cleared");
}

void populate_tree_header(index_header_record *bh) {
  if (bh == NULL) {
    puts("!!Header pointer is NULL, cannot populate");
    return;
  }

  bh->page_size = sizeof(page);
  bh->root_rrn = 999;

}

void create_new_tree(b_tree_buf *b, io_buf *data, io_buf *index, int n) {
  data_record *d;
  page *p;
  p = malloc(sizeof(page));
  d = malloc(sizeof(data_record));
  for(int i = 0; i < n; i++) {
    d = read_data_record(data, i);
    key key;
    key.data_register_rrn = i;
    memcpy(key.id, d->placa, TAMANHO_PLACA);
    insert_key(b, index, b->root, key ,p);
  }
  free(p);
  free(d);
  return;
}

u16 search_key(page *page, key key, int *return_pos) {
  if(!page)
    return ERROR;

  for(int i = 0; i < ORDER-1; i++) {
    if(memcpy(page->keys[i].id, key.id, TAMANHO_PLACA) == 0) {
      *return_pos = i;
      return FOUND;
    }
  }
  *return_pos = NOT_FOUND;
  return NOT_FOUND;
}

page *load_page(io_buf *io, queue *q, u16 rrn) {
  if(io) {
    puts("!!Error while loading page");
    return NULL;
  }

  page *q_page = queue_search(q, rrn);
  if(q_page != NULL){
    puts("@Page found on queue");
    return q_page;
  }

  int byte_offset = (rrn * io->b->page_size) + sizeof(index_header_record);
  fseek(io->fp, byte_offset, SEEK_SET);

  page *page = alloc_page();
  if(page)
    fread(page, io->b->page_size ,1, io->fp);
  return page;
}

void driver(){}


u16 search(b_tree_buf *b, io_buf *io, page *p, key key, u16 *found_rrn, u16 *found_pos, page *return_page) {
  if (p == NULL) {
    puts("!!Error: NULL root");
    return ERROR;
  }       

  page *temp = p;
  int rrn = temp->rrn;  
  int pos;

  int flag = search_key(temp, key, &pos);
  if (flag == FOUND) {
    *found_rrn = rrn;
    *found_pos = pos;
    *return_page = *p; 
    return FOUND;
  }

  page *child_page = load_page(io, b->q, temp->children[pos]);

  if (child_page == NULL) { 
    puts("!!Error: Failed to load child page");
    return ERROR;
  }

  return search(b, io, child_page, key, found_rrn, found_pos, return_page); 
  // TODO update and test this shit
}


void driver();


u16 insert_key(b_tree_buf *b, io_buf *io, page *p, key key, page *return_page){

  return PROMOTION;
}

page* split(page *page);  // TODO

int promote(); // TODO

page *redistribute();  // TODO

int remove_key(io_buf *io, b_tree_buf *b, page *page) {
  return false; // TODO
}

void print_page(page *page) {
  

}

void write_index_header(io_buf *io) {
  if (!io->fp) {
    puts("!!NULL file");
    exit(-1);
  }

  if(io->b->page_size == 0) {
    puts("!!Error: page size == 0");
    return;
  }

  if (io->b == NULL) {
    io->b = malloc(sizeof(data_header_record));
    if (io->b == NULL) {
      puts("!!Memory allocation error");
      return;
    }
  }

  fseek(io->fp, 0, SEEK_SET);
  int flag;
  flag = fwrite(io->b, sizeof(index_header_record), 1 ,io->fp);

  if (flag != 1){
    puts("!!Error while writing to file");
    if(DEBUG) {
      printf("Flag: %d\n", flag);
      printf("Header: root: %hu page size: %hu\n", io->b->root_rrn, io->b->page_size);
    }
    return;
  }

  if (DEBUG)
    puts("@Successfully written on index");
}

void read_index_header(io_buf *io) {
  if (!io->fp) {
    puts("!!File not opened");
    return;
  }

  index_header_record *hr;
  fseek(io->fp, 0, SEEK_SET);
  hr = malloc(sizeof(index_header_record));
  int t;
  t = fread(hr, sizeof(index_header_record), 1, io->fp);

  if (t != 1) {
    puts("!!Error while reading header record");
    printf("t size: %d\n", t);
    printf("Read sizes: %hu %hu \n", hr->page_size, hr->root_rrn);
    return;
  }

  io->b->root_rrn = hr->root_rrn;
  io->b->page_size = hr->page_size;

  if(DEBUG) {
    puts("@Index header Record Loaded");
    printf("-->index_header: root_rrn: %hu page_size: %hu\n", io->b->page_size, io->b->root_rrn);
  }
}

void create_index_file(io_buf *io, char *file_name) {
  strcpy(io->address, file_name);
  if (io->hr == NULL) {
    puts("!!Memory allocation failed for data_header_record");
    return;  
  }

  if(io->fp != NULL || !io)
    exit(-1);

  io->fp = fopen(io->address, "r+b");
  if (!io->fp) {
    printf("!!Error opening file: %s", io->address);
    return; 
  }

  populate_tree_header(io->b);
  if(io->hr != NULL) {
    write_index_header(io);
    return;
  }
}
