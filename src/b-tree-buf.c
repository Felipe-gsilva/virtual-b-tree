#include "b-tree-buf.h"
#include "free-rrn-list.h"
#include "io-buf.h"
#include "queue.h"

b_tree_buf *alloc_tree_buf() {
  b_tree_buf *b = malloc(sizeof(b_tree_buf));
  if (!b) {
    puts("!!Could not allocate b_tree_buf_BUFFER");
    return NULL;
  }

  b->root = NULL;
  b->io = alloc_io_buf();
  if (!b->io) {
    free(b);
    puts("!!Could not allocate io_buf");
    return NULL;
  }

  b->q = alloc_queue();
  if (!b->q) {
    free(b->io);
    free(b);
    puts("!!Could not allocate queue");
    return NULL;
  }

  b->i = alloc_ilist();
  if (!b->i) {
    free(b->q);
    free(b->io);
    free(b);
    puts("!!Could not allocate ilist");
    return NULL;
  }

  if (DEBUG)
    puts("@Allocated b_tree_buf_BUFFER");
  return b;
}

void clear_tree_buf(b_tree_buf *b) {
  if (b) {
    clear_io_buf(b->io);
    clear_queue(b->q);
    clear_ilist(b->i);
    if (b->root) {
      clear_page(b->root);
      b->root = NULL;
    }
    free(b);
    b = NULL;
  }
  if (DEBUG)
    puts("@B_TREE_BUFFER cleared");
}

void populate_index_header(index_header_record *bh, char *file_name) {
  if (bh == NULL) {
    puts("!!Header pointer is NULL, cannot populate");
    return;
  }

  bh->page_size = sizeof(page);
  bh->root_rrn = 0;
  strcpy(bh->free_rrn_address, file_name);
  bh->free_rrn_address[strlen(file_name) + 1] = '\0';
  bh->header_size = (sizeof(u16) * 3) + strlen(file_name) + 1;
}
void build_tree(b_tree_buf *b, io_buf *data, int n) {
  if (!b || !data) {
    puts("!!Invalid parameters");
    return;
  }
  if (!b->i) {
    load_list(b->i, b->io->br->free_rrn_address);
    if (b->i && DEBUG) {
      puts("@Loaded rrn list");
    }
  }

  for (int i = 0; i < n; i++) {
    data_record *d = load_data_record(data, i);
    if (!d) {
      printf("!!Failed to load record %d\n", i);
      continue;
    }
    if (DEBUG) {
      print_data_record(d);
    }

    btree_status status = b_insert(b, data, d, i);
    if (status != BTREE_SUCCESS && status != BTREE_INSERTED_IN_PAGE) {
      printf("!!Failed to insert record %d, error: %d\n", i, status);
    }
    free(d);
  }

  if (DEBUG) {
    puts("@Built tree");
  }
}

void test_tree(b_tree_buf *b, io_buf *data, int n) {
  if (!b || !data) {
    puts("!!Invalid parameters");
    return;
  }
  if (!b->i) {
    load_list(b->i, b->io->br->free_rrn_address);
    if (b->i && DEBUG) {
      puts("@Loaded rrn list");
    }
  }

  page *p = alloc_page();
  for (int i = 0; i < n; i++) {
    data_record *d = load_data_record(data, i);
    if (!d) {
      printf("!!Failed to load record %d\n", i);
      continue;
    }
    puts("--------------");
    printf("TEST NUM %d\n", i);
    printf("tested ids %s\n", d->placa);

    p = b_search(b, d->placa);
    if (!p) {
      return;
    }
    print_page(p);
    free(d);
  }
  clear_page(p);

  if (DEBUG) {
    puts("@Built tree");
  }
}

page *load_page(b_tree_buf *b, u16 rrn) {
  if (!b->io) {
    puts("!!Error while loading page");
    return NULL;
  }

  page *q_page = alloc_page();
  q_page = queue_search(b->q, rrn);
  if (q_page != NULL) {
    if (DEBUG)
      puts("@Page found on queue");
    return q_page;
  }
  if(!q_page)
    free(q_page);

  int byte_offset = (b->io->br->header_size) + ((b->io->br->page_size) * rrn);

  if (fseek(b->io->fp, byte_offset, SEEK_SET)) {
    puts("!!Error: could not fseek");
    return NULL;
  }

  page *page = alloc_page();
  if (fread(page, b->io->br->page_size, 1, b->io->fp) != 1) {
    puts("!!Error: could not read page");
    return NULL;
  }

  if (page)
    push_page(b, page);

  return page;
}

int write_root_rrn(b_tree_buf *b, u16 rrn) {
  if (!b) {
    puts("!!Error: NULL b_tree_buf");
    return BTREE_ERROR_IO;
  }
  if (b->root->rrn == rrn) {
    puts("@RRN already refers to root");
    return BTREE_SUCCESS;
  }

  page *temp = load_page(b, rrn);
  if (!temp) {
    puts("!!Error: page to be inserted do not exists");
    return BTREE_ERROR_INVALID_PAGE;
  }
  fseek(b->io->fp, 0, SEEK_SET);
  int flag = fwrite(&temp->rrn, sizeof(u16), 1, b->io->fp);
  if (flag != 1) {
    puts("!!Error: Could not update root rrn");
    exit(-1);
  }

  fflush(b->io->fp);
  return BTREE_SUCCESS;
}

void b_traverse(b_tree_buf *b, page *p) {
  int i;
  for (i = 0; i < p->child_num - 1; i++) {
    if (p->leaf == false)
      b_traverse(b, load_page(b, p->children[i]));
    printf("CHAVES: %s", p->keys[i].id);
  }
  if (p->leaf == false)
    b_traverse(b, load_page(b, p->children[i]));
}

page *b_search(b_tree_buf *b, const char *s) {
  u16 *pos = malloc(sizeof(u16));
  page *return_page = NULL;
  key key;
  strcpy(key.id, s);
  if (!b->root)
    b->root = load_page(b, b->io->br->root_rrn);

  int flag = search_key(b, b->root, key, pos, &return_page);
  if (flag == BTREE_FOUND_KEY) {
    free(pos);
    return return_page;
  }
  puts("--> Could not find key");
  free(pos);
  return NULL;
}

int search_in_page(page *p, key key, int *return_pos) {
  if (!p) {
    puts("!!Error: no page");
    return BTREE_ERROR_INVALID_PAGE;
  }

  for (int i = 0; i < ORDER - 1; i++) {
    if (p->keys[i].id[0] == '\0') {
      *return_pos = i;
      return BTREE_NOT_FOUND_KEY;
    }

    printf("page key id: %s\t key id: %s\n", p->keys[i].id, key.id);
    if (strcmp(p->keys[i].id, key.id) == 0) {
      puts("@Curr key was found");
      *return_pos = i;
      return BTREE_FOUND_KEY;
    }

    if (strcmp(p->keys[i].id, key.id) > 0) {
      *return_pos = i;
      puts("@Curr key is greater than the new one");
      return BTREE_NOT_FOUND_KEY;
    }
  }

  *return_pos = ORDER - 1;
  return BTREE_NOT_FOUND_KEY;
}

u16 search_key(b_tree_buf *b, page *p, key key, u16 *found_pos,
               page **return_page) {
  if (!b || !found_pos || !return_page || !p) {
    puts("!!Error: NULL parameters");
    return BTREE_ERROR_INVALID_PAGE;
  }

  page *current = p;
  while (current != NULL) {
    int pos;
    int flag = search_in_page(current, key, &pos);

    if (flag == BTREE_FOUND_KEY) {
      *found_pos = pos;
      *return_page = current;
      return BTREE_FOUND_KEY;
    }

    if (current->leaf || current->children[pos] == (u16)-1) {
      if (current != p)
        free(current);
      *return_page = NULL;
      return BTREE_NOT_FOUND_KEY;
    }

    page *next = load_page(b, current->children[pos]);
    if (!next) {
      if (current != p)
        free(current);
      *return_page = NULL;
      return BTREE_ERROR_IO;
    }

    if (current != p)
      free(current);
    current = next;
  }

  *return_page = NULL;
  return BTREE_NOT_FOUND_KEY;
}

void populate_key(key *k, data_record *d, u16 rrn) {
  if (!k || !d) {
    puts("!!Error: NULL key and data record");
    return;
  }
  memcpy(k->id, d->placa, strlen(d->placa));
  k->id[strlen(d->placa)] = '\0';
  k->data_register_rrn = rrn;
}

btree_status insert_in_page(page *p, key k, page *r_child, int pos) {
  if (!p) {
    if (DEBUG)
      puts("!!Error: page NULL");
    return BTREE_ERROR_INVALID_PAGE;
  }

  if (pos < 0 || pos > p->keys_num) {
    if (DEBUG)
      puts("!!Error: invalid position");
    return BTREE_ERROR_INVALID_PAGE;
  }

  if (p->keys_num >= ORDER - 1) {
    if (DEBUG)
      puts("!!Error: page overflow");
    return BTREE_ERROR_PAGE_FULL;
  }

  if (DEBUG) {
    printf("Current state - keys: %hu, children: %hu, inserting at pos: %d\n",
           p->keys_num, p->child_num, pos);
  }

  for (int i = p->keys_num - 1; i >= pos; i--) {
    if (i + 1 < ORDER - 1) {
      p->keys[i + 1] = p->keys[i];
    }
  }

  p->keys[pos] = k;
  p->keys_num++;

  if (!p->leaf) {
    for (int i = p->child_num - 1; i >= pos; i--) {
      if (i + 1 < ORDER) {
        p->children[i + 1] = p->children[i];
      }
    }

    if (r_child) {
      p->children[pos + 1] = r_child->rrn;
      p->child_num++;
    }
  } else {
    if (r_child && DEBUG)
      puts("Warning: r_child provided for leaf node");
  }

  if (DEBUG)
    printf("After insertion - keys: %hu, children: %hu\n", p->keys_num,
           p->child_num);

  return BTREE_INSERTED_IN_PAGE;
}

btree_status b_insert(b_tree_buf *b, io_buf *data, data_record *d, u16 rrn) {
  if (!b || !data || !d || rrn == (u16)-1) {
    return BTREE_ERROR_INVALID_PAGE;
  }

  key *k = malloc(sizeof(key));
  if (!k)
    return BTREE_ERROR_MEMORY;
  populate_key(k, d, rrn);

  if (!b->root) {
    b->root = load_page(b, b->io->br->root_rrn);
    if (!b->root) {
      b->root = alloc_page();
      if (!b->root) {
        free(k);
        return BTREE_ERROR_MEMORY;
      }
      b->root->rrn = get_free_rrn(b->i);
    }
  }

  key *return_key = malloc(sizeof(key));
  if (!return_key) {
    free(k);
    return BTREE_ERROR_MEMORY;
  }

  page *return_page = NULL;
  btree_status flag = insert_key(b, b->root, *k, return_key, &return_page);

  if (flag == BTREE_PROMOTION) {
    if (DEBUG)
      puts("@Creating new root due to promotion");

    page *new_root = alloc_page();
    if (!new_root) {
      free(k);
      free(return_key);
      return BTREE_ERROR_MEMORY;
    }

    new_root->leaf = false;
    new_root->keys_num = 1;
    new_root->child_num = 2;
    new_root->keys[0] = *return_key;
    new_root->children[0] = b->root->rrn;
    new_root->children[1] = return_page->rrn;

    u16 new_rrn = get_free_rrn(b->i);
    if (new_rrn == (u16)-1) {
      free(k);
      free(return_key);
      return BTREE_ERROR_IO;
    }
    b->root = new_root;
    b->root->rrn = new_rrn;

    if (write_root_rrn(b, b->root->rrn) != BTREE_SUCCESS ||
        write_index_record(b, b->root) != BTREE_SUCCESS) {
      free(k);
      free(return_key);
      return BTREE_ERROR_IO;
    }

    flag = BTREE_SUCCESS;
  }
  if (flag != BTREE_SUCCESS && flag != BTREE_ERROR_DUPLICATE && flag != BTREE_INSERTED_IN_PAGE) {
    if (DEBUG)
      printf("@Error during insertion: %d\n", flag);
  } else {
    if (DEBUG)
      puts("@Normal insertion completed");
  }

  free(k);
  free(return_key);
  return flag;
}

btree_status b_split(b_tree_buf *b, page *p, page **r_child, key *promo_key,
                     key *incoming_key) {
  key temp_keys[ORDER];
  u16 temp_children[ORDER + 1];

  memcpy(temp_keys, p->keys, p->keys_num * sizeof(key));

  int i = 0;
  for (i = 0; i < p->keys_num && strcmp(p->keys[i].id, incoming_key->id) < 0; i++){
    memmove(&temp_keys[i + 1], &temp_keys[i],(p->keys_num - i) * sizeof(key));
  }
  temp_keys[i] = *incoming_key;


  if (!p->leaf) {
    memcpy(temp_children, p->children, p->child_num * sizeof(u16));
    memmove(&temp_children[i + 1], &temp_children[i], (p->child_num - i - 1) * sizeof(u16));
    temp_children[i] = ((*r_child) != NULL) ? (*r_child)->rrn : (u16)-1;
  }

  int split =(int) (ORDER / 2);

  page *new_page = alloc_page();
  if (!new_page)
    return BTREE_ERROR_MEMORY;

  new_page->rrn = get_free_rrn(b->i);
  if (new_page->rrn == (u16)-1) {
    free(new_page);
    return BTREE_ERROR_IO;
  }

  *promo_key = temp_keys[split];

  p->keys_num = 0;
  for (i = 0; i < split; i++) {
    p->keys[p->keys_num++] = temp_keys[i];
  }

  new_page->leaf = p->leaf;
  for (i = split + 1; i <= ORDER - 1; i++) {
    new_page->keys[new_page->keys_num++] = temp_keys[i];
  }

  if (!p->leaf) {
    p->child_num = split + 1;
    new_page->child_num = 0;

    for (i = 0; i <= split; i++) {
      p->children[i] = temp_children[i];
    }
    for (i = split + 1; i <= ORDER; i++) {
      new_page->children[new_page->child_num++] = temp_children[i];
    }
  }

  btree_status status;
  if ((status = write_index_record(b, p)) != BTREE_SUCCESS ||
      (status = write_index_record(b, new_page)) != BTREE_SUCCESS) {
    return status;
  }

  *r_child = new_page;

  return BTREE_PROMOTION;
}

btree_status insert_key(b_tree_buf *b, page *p, key k, key *promo_key,
                        page **r_child) {
  if (!b || !promo_key || !p)
    return BTREE_ERROR_INVALID_PAGE;

  int pos;
  btree_status status = search_in_page(p, k, &pos);
  if (status == BTREE_FOUND_KEY)
    return BTREE_ERROR_DUPLICATE;

  if (!p->leaf) {
    page *child = load_page(b, p->children[pos]);
    if (!child)
      return BTREE_ERROR_IO;

    key temp_key;
    page *temp_child = NULL;
    status = insert_key(b, child, k, &temp_key, &temp_child);

    if (status == BTREE_PROMOTION) {
      k = temp_key;
      if (p->keys_num < ORDER - 1) {
        return insert_in_page(p, k, temp_child, pos);
      }
      return b_split(b, p, r_child, promo_key, &k);
    }
    return status;
  }

  if (p->keys_num < ORDER - 1) {
    return insert_in_page(p, k, NULL, pos);
  }

  return b_split(b, p, r_child, promo_key, &k);
}

page *redistribute(); // TODO

void b_remove(b_tree_buf *b, io_buf *data, char *s) {
  // TODO
}

int remove_key(b_tree_buf *b, page *page) {
  return false; // TODO
}

void print_page(page *page) {
  puts("\n----------PAGE----------");
  printf("page rrn: %hu\n", page->rrn);
  for (int i = 0; i < page->keys_num; i++) {
    printf("key id: %s | data_rrn: %hu;\t", page->keys[i].id,
           page->keys[i].data_register_rrn);
  }
  printf("\n");
  printf("children rrn: ");
  for (int i = 0; i < page->child_num; i++) {
    printf("i:%d= %hu\t", i, page->children[i]);
  }
  printf("\nchild_number %hu\n", page->child_num);
  printf("keys number %hu\n", page->keys_num);
  if (page->leaf)
    puts("This page is a leaf");
  puts("------------------------\n");
}

int write_index_header(io_buf *io) {
  if (!io || !io->fp) {
    puts("!!NULL file");
    return BTREE_ERROR_IO;
  }
  if (io->br == NULL) {
    io->br = malloc(sizeof(index_header_record));
    if (io->br == NULL) {
      puts("!!Memory allocation error");
      return BTREE_ERROR_MEMORY;
    }
  }
  if (io->br->page_size == 0) {
    puts("!!Error: page size == 0");
    return BTREE_ERROR_INVALID_PAGE;
  }

  size_t free_rrn_len = strlen(io->br->free_rrn_address) + 1;
  io->br->header_size = sizeof(u16) * 3 + free_rrn_len;

  fseek(io->fp, 0, SEEK_SET);

  if (fwrite(&io->br->root_rrn, sizeof(u16), 1, io->fp) != 1) {
    puts("!!Error while writing root_rrn");
    return BTREE_ERROR_IO;
  }

  if (fwrite(&io->br->page_size, sizeof(u16), 1, io->fp) != 1) {
    puts("!!Error while writing page_size");
    return BTREE_ERROR_IO;
  }

  if (fwrite(&io->br->header_size, sizeof(u16), 1, io->fp) != 1) {
    puts("!!Error while writing size");
    return BTREE_ERROR_IO;
  }

  if (fwrite(io->br->free_rrn_address, free_rrn_len, 1, io->fp) != 1) {
    puts("!!Error while writing free_rrn_address");
    return BTREE_ERROR_IO;
  }

  if (DEBUG) {
    printf("@Successfully written on index: root_rrn: %hu, page_size: %hu, "
           "size: %hu, "
           "free_rrn_address: %s\n",
           io->br->root_rrn, io->br->page_size, io->br->header_size,
           io->br->free_rrn_address);
  }

  fflush(io->fp);
  return BTREE_SUCCESS;
}

void load_index_header(io_buf *io) {
  if (!io || !io->fp) {
    puts("!!Invalid IO buffer or file pointer");
    return;
  }

  if (!io->br) {
    io->br = malloc(sizeof(index_header_record));
    if (!io->br) {
      puts("!!Memory allocation error");
      return;
    }
    memset(io->br, 0, sizeof(index_header_record));
  }

  fseek(io->fp, 0, SEEK_SET);

  if (fread(&io->br->root_rrn, sizeof(u16), 1, io->fp) != 1) {
    puts("!!Error reading root_rrn");
    return;
  }

  if (fread(&io->br->page_size, sizeof(u16), 1, io->fp) != 1) {
    puts("!!Error reading page_size");
    return;
  }

  if (fread(&io->br->header_size, sizeof(u16), 1, io->fp) != 1) {
    puts("!!Error reading size");
    return;
  }

  printf("root_rrn: %hu, page_size: %hu, size: %hu\n", io->br->root_rrn,
         io->br->page_size, io->br->header_size);

  size_t rrn_len = io->br->header_size - (3 * sizeof(u16));

  io->br->free_rrn_address = malloc(rrn_len + 1);
  if (!io->br->free_rrn_address) {
    puts("!!Memory allocation error for free_rrn_address");
    return;
  }

  if (fread(io->br->free_rrn_address, rrn_len, 1, io->fp) != 1) {
    puts("!!Error reading free_rrn_address");
    free(io->br->free_rrn_address);
    io->br->free_rrn_address = NULL;
    return;
  }
  io->br->free_rrn_address[rrn_len] = '\0';

  if (DEBUG) {
    puts("@Index header Record Loaded");
    printf("-->index_header: root_rrn: %hu page_size: %hu size: %hu "
           "free_rrn_list: %s\n",
           io->br->root_rrn, io->br->page_size, io->br->header_size,
           io->br->free_rrn_address);
  }
}

int write_index_record(b_tree_buf *b, page *p) {
  if (!b->io || !p) {
    puts("!!Error: invalid parameters");
    return BTREE_ERROR_IO;
  }

  if (!b->io->br || !b->io->fp) {
    puts("!!Error: invalid buffer or file pointer");
    return BTREE_ERROR_MEMORY;
  }

  int byte_offset = (b->io->br->header_size) + ((b->io->br->page_size) * p->rrn);
  if (DEBUG) {
    puts("////////");
    printf("@Writting following page: \n");
    print_page(p);
    printf("byte_offset %d\n", byte_offset);
  }

  if (fseek(b->io->fp, byte_offset, SEEK_SET) != 0) {
    puts("!!Error: seek operatb->ion failed");
    return BTREE_ERROR_IO;
  }

  if (fwrite(p, b->io->br->page_size, 1, b->io->fp) != 1) {
    puts("!!Error: write operatb->ion failed");
    return BTREE_ERROR_IO;
  }

  if (DEBUG)
    printf("@Successfully wrote page %hu at offset %d\n", p->rrn, byte_offset);

  fflush(b->io->fp);
  page *q_page = queue_search(b->q, p->rrn);
  if(p == q_page)
    push_page(b, p);

  return BTREE_SUCCESS;
}

void create_index_file(io_buf *io, char *file_name) {
  if (!io || !file_name) {
    puts("!!Invalid io buffer or file name");
    return;
  }

  strcpy(io->address, file_name);

  if (io->hr == NULL) {
    io->hr = malloc(sizeof(data_header_record));
    if (io->hr == NULL) {
      puts("!!Memory allocation failed for data_header_record");
      return;
    }
  }

  if (io->br == NULL) {
    free(io->hr);
    io->hr = NULL;
    io->br = malloc(sizeof(index_header_record));
    if (io->br == NULL) {
      puts("!!Memory allocation failed for index_header_record");
      return;
    }
    io->br->free_rrn_address = malloc(sizeof(char) * MAX_ADDRESS);
    if (io->br->free_rrn_address == NULL) {
      puts("!!Memory allocation failed for free_rrn_address");
      return;
    }
  }

  if (io->fp != NULL) {
    puts("!!File already opened");
    return;
  }

  printf("Loading file: %s\n", io->address);
  io->fp = fopen(io->address, "r+b");
  if (!io->fp) {
    printf("!!Error opening file: %s. Creating it...\n", io->address);
    io->fp = fopen(io->address, "wb");
    if (io->fp) {
      fclose(io->fp);
    }
    io->fp = fopen(io->address, "r+b");
    if (!io->fp) {
      puts("!!Failed to create and open file");
      return;
    }
  }

  char list_name[MAX_ADDRESS];
  strcpy(list_name, file_name);
  char *dot = strrchr(list_name, '.');
  if (dot) {
    strcpy(dot, ".hlp");
  }

  strcpy(io->br->free_rrn_address, list_name);

  populate_index_header(io->br, io->br->free_rrn_address);
  write_index_header(io);

  if (DEBUG) {
    puts("@Index file created successfully");
  }
}

page *alloc_page() {
  page *p = malloc(sizeof(page));
  if (p) {
    memset(p, 0, sizeof(page));
    p->leaf = true;
    for (int i = 0; i < ORDER - 1; i++)
      p->children[i] = (u16)-1;
  }
  return p;
}

page *new_page(u16 rrn) {
  page *page = alloc_page();
  page->rrn = rrn;
  if (page)
    return page;
  puts("!!Error: could not creat a page");
  return NULL;
}

void clear_page(page *page) {
  if (page) {
    free(page);
    if (DEBUG)
      puts("@Successfully freed page");
    return;
  }
  puts("Error while freeing page");
}
