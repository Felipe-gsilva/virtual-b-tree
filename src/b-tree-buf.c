#include "b-tree-buf.h"
#include "io-buf.h"
#include "queue.h"
#include "i_list.h"

b_tree_buf *alloc_tree_buf() {
  b_tree_buf *b = malloc(sizeof(b_tree_buf));
  b->root = alloc_page();
  b->io = alloc_io_buf();
  b->q = alloc_queue();
  b->i = alloc_ilist();
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
    clear_ilist(b->i);
    if(b->root) {
      clear_page(b->root);
      b->root = NULL;
    }
    free(b);
    b = NULL;
  }
  if(DEBUG)
    puts("@B_TREE_BUFFER cleared");
}

void populate_tree_header(index_header_record *bh, char *file_name) {
  if (bh == NULL) {
    puts("!!Header pointer is NULL, cannot populate");
    return;
  }

  bh->page_size = sizeof(page);
  bh->root_rrn = 0;
  strcpy(bh->free_rrn_address, file_name);
}

void build_tree(b_tree_buf *b, io_buf *data, int n) {
  data_record *d;
  page *p, **pc;
  key pb;
  p = malloc(sizeof(page));
  pc = malloc(sizeof(page*));
  d = malloc(sizeof(data_record));
  print_page(b->root);
  load_list(b->i, b->io->b->free_rrn_address);
  for(int i = 0; i < n; i++) {
    d = read_data_record(data, i);
    key key;
    key.data_register_rrn = i;
    memcpy(key.id, d->placa, TAMANHO_PLACA);
    int flag = insert_key(b, b->root, key, &pb, pc);
  }
  free(p);
  free(pc);
  free(d);
  return;
}


page *load_page(io_buf *io, queue *q, u16 rrn) {
  if(!io) {
    puts("!!Error while loading page");
    return NULL;
  }

  page *q_page = queue_search(q, rrn);
  if(q_page != NULL){
    if(DEBUG)
      puts("@Page found on queue");
    return q_page;
  }

  int byte_offset = (rrn * io->b->page_size) + sizeof(index_header_record);
  fseek(io->fp, byte_offset, SEEK_SET);

  page *page = alloc_page();
  fread(page, io->b->page_size ,1, io->fp);

  if(page) 
    push_page(q, page);
      
  return page;
}

page *search(b_tree_buf *b, const char *s) {
  u16 pos;
  page *return_page;
  key key;
  strcpy(key.id, s);

  int flag = search_key(b, b->root, key, &pos, return_page);
  if(flag == FOUND)
    return return_page;
  return NULL;
}

u16 search_page(page *page, key key, int *return_pos) {
  if(!page)
    return ERROR;

  for(int i = 0; i < ORDER-1; i++) {
    if(memcmp(page->keys[i].id, key.id, TAMANHO_PLACA) == 0) {
      *return_pos = i;
      return FOUND;
    }
    if(memcmp(page->keys[i].id, key.id, TAMANHO_PLACA) < 0) {
      puts("chave atual maior");
      *return_pos = i - 1;
      return NOT_FOUND;
    }
  }
  return NOT_FOUND;
}

u16 search_key(b_tree_buf *b, page *p, key key, u16 *found_pos, page *return_page) {
  if (b == NULL) {
    puts("!!Error: NULL root");
    return ERROR;
  }       
  if(p == NULL) {
    return NOT_FOUND;
  }
  page *temp = p;
  int pos;

  int flag = search_page(temp, key, &pos);
  if (flag == FOUND) {
    *found_pos = pos;
    *return_page = *temp; 
    return FOUND;
  }

  temp = load_page(b->io, b->q, temp->children[pos]);

  if (temp == NULL) { 
    puts("!!Error: Failed to load child page");
    return NOT_FOUND;
  }

  return search_key(b, temp ,key, found_pos, return_page); 
}


void driver() { } // TODO

void insert(b_tree_buf *b,io_buf *data,  data_record *d) { } // TODO

void insert_in_page(page *p, key k, page *r_child, int pos) {
  p->child_number += 1;
  p->keys[pos] = k;
  p->children[pos+1] = r_child->rrn;
}

void split(page *p, key k, page *r_child, key *promo_key, page *new_page, int pos) { } // TODO 

u16 insert_key(b_tree_buf *b, page *p, key k, key *promo_key, page **r_child) {
  page *temp;
  key promo;
  int flag, pos;

  if (!p) {
    *promo_key = k;
    *r_child = NULL;
    return PROMOTION;
  }

  flag = search_page(p, k, &pos);
  printf("flag from search_key: %d\n", flag);
  if (flag == FOUND) {
    puts("!!Error: key already inserted");
    return ERROR;
  }

  temp = load_page(b->io, b->q, p->children[pos]);
  flag = insert_key(b, temp, k, &promo, r_child);

  if (flag == NO_PROMOTION || flag == ERROR) {
    puts("!!Error: no promotion or error");
    return flag;
  }

  if (p->child_number < ORDER - 1) {
    if(DEBUG)
      puts("@Free space on page. Inserting..");
    insert_in_page(p, promo, *r_child, pos);
    return NO_PROMOTION;
  }
  
  key new_promo_key;
  page *new_page = alloc_page(); 
  split(p, promo, *r_child, &new_promo_key, new_page, pos);
  *promo_key = new_promo_key;
  *r_child = new_page;

  if(DEBUG)
    puts("@Inserted");
  return PROMOTION;
}

int promote() {return 0;} // TODO

page *redistribute();  // TODO

int remove_key(b_tree_buf *b, page *page) {
  return false; // TODO
}

void print_page(page *page) { // TODO
  printf("");

}
void print_tree(b_tree_buf *b) { // TODO

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
  strcpy(io->b->free_rrn_address, hr->free_rrn_address);

  if(DEBUG) {
    puts("@Index header Record Loaded");
    printf("-->index_header: root_rrn: %hu page_size: %hu free rrn list %s\n", io->b->root_rrn,io->b->page_size,  io->b->free_rrn_address);
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

  char list_name[MAX_ADDRESS]; 
  strcpy(list_name, file_name);
  char *dot = strrchr(list_name, '.');
  if (dot) {
    strcpy(dot, ".hlp"); // TODO move this from here
  }
  populate_tree_header(io->b, list_name);
  if(io->hr != NULL) {
    write_index_header(io);
    return;
  }
}

page *alloc_page() {
    page* newpage = malloc(sizeof(page));
    if(newpage)
        return newpage;
    return NULL;
}

page *new_page(u16 rrn, key keys[], u16 children[]) {
    page *page = alloc_page();
    page->rrn = rrn;   
    if(keys && children) {
        for(int i = 0; i < ORDER - 1; i++) {
            page->keys[i] = keys[i];
            page->children[i] = children[i];
        }
        page->children[ORDER-1] = children[ORDER-1];
    }
    if(page)
        return page;
    return NULL;
}


void clear_page(page *page) {
    if(page) {
        free(page);
        if(DEBUG)
            puts("@Successfully freed page");
        return;
    }
    puts("error while freeing page");
}
