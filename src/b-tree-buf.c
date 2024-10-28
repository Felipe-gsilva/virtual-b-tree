#include "b-tree-buf.h"
#include "free_rrn_list.h"
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

void populate_tree_header(index_header_record *bh, char *file_name) {
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
  data_record d;
  if (!b->i)
    load_list(b->i, b->io->br->free_rrn_address);

  if (b->i)
    if (DEBUG)
      puts("@Loaded rrn list");
  for (int i = 0; i < n; i++) {
    d = *load_data_record(data, i);
    print_data_record(&d);
    b_insert(b, data, &d, i);
  }
  if (DEBUG)
    puts("@Built tree");
}

page *load_page(b_tree_buf *b, u16 rrn) {
  if (!b->io) {
    puts("!!Error while loading page");
    return NULL;
  }

  page *q_page = queue_search(b->q, rrn);
  if (q_page != NULL) {
    if (DEBUG)
      puts("@Page found on queue");
    return q_page;
  }

  int byte_offset = (b->io->br->header_size) + ((b->io->br->page_size) * rrn);

  if (fseek(b->io->fp, byte_offset, SEEK_SET)) {
    puts("!!Error: could not fseek");
    return NULL;
  }

  page *page = alloc_page();
  fread(page, b->io->br->page_size, 1, b->io->fp);

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
  if (flag) {
    puts("!!Error: Could not update root rrn");
    exit(-1);
  }
  return BTREE_SUCCESS;
}

void b_traverse(b_tree_buf *b, page *p) {
  int i;
  for (i = 0; i < p->child_number - 1; i++) {
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

int search_in_page(page *page, key key, int *return_pos) {
  if (!page || !page->rrn) {
    puts("!!Error: no page or page rrn");
    return BTREE_ERROR_INVALID_PAGE;
  }

  for (int i = 0; i < ORDER - 1; i++) {
    printf("page key id: %s\t key id: %s\n", page->keys[i].id, key.id);
    if (strcmp(page->keys[i].id, key.id) == 0) {
      puts("@Curr key was found");
      *return_pos = i;
      return BTREE_FOUND_KEY;
    }
    if (strcmp(page->keys[i].id, key.id) > 0) {
      puts("@Curr key is greater than the new one");
      *return_pos = (i - 1 <= 0) ? 0 : i - 1;
      printf("i: %d\n", i);
      return BTREE_NOT_FOUND_KEY;
    }
  }
  return BTREE_NOT_FOUND_KEY;
}

u16 search_key(b_tree_buf *b, page *p, key key, u16 *found_pos,
               page **return_page) {
  if (!b || !found_pos || !return_page) {
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

    if (current->leaf || current->children[pos] == 0) {
      if (current != p)
        free(current);
      return BTREE_NOT_FOUND_KEY;
    }

    page *next = load_page(b, current->children[pos]);
    if (!next) {
      if (current != p)
        free(current);
      return BTREE_ERROR_IO;
    }

    if (current != p)
      free(current);
    current = next;
  }

  return BTREE_NOT_FOUND_KEY;
}
void populate_key(key *k, data_record *d, u16 rrn) {
  if (!k || !d) {
    puts("!!Error: NULL key and data record");
    return;
  }
  memcpy(k->id, d->placa, TAMANHO_PLACA);
  k->data_register_rrn = rrn;
}

btree_status b_insert(b_tree_buf *b, io_buf *data, data_record *d, u16 rrn) {
  if (!b || !data || !d) {
    return BTREE_ERROR_INVALID_PAGE;
  }

  key k;
  key *return_key = malloc(sizeof(key));
  if (return_key == NULL)
    return BTREE_ERROR_MEMORY;

  page *return_page = NULL;
  btree_status status = 0;

  populate_key(&k, d, rrn);
  int flag = insert_key(b, b->root, k, return_key, &return_page);
  printf("flag from first insert %d\n", flag);

  if (flag == BTREE_PROMOTION) {
    page *new_root = alloc_page();
    if (new_root == NULL) {
      free(return_key);
      return BTREE_ERROR_MEMORY;
    }

    if (!b->root) {
      b->root = new_page(0);
      if (!b->root) {
        free(return_key);
        free(new_root);
        return BTREE_ERROR_MEMORY;
      }
    }

    if (new_root->child_number >= ORDER - 1) {
      free(return_key);
      free(new_root);
      return BTREE_ERROR_INVALID_PAGE;
    }

    new_root->keys[0] = *return_key;
    new_root->children[0] = b->root->rrn;
    new_root->child_number = 1;

    if (return_page) {
      new_root->children[1] = return_page->rrn;
      new_root->child_number++;
      free(return_page);
    }

    insert_list(b->i, get_last_free_rrn(b->i) + 1);
    u16 new_rrn = get_free_rrn(b->i);
    printf("@new rrn to be added %hu\n", new_rrn);

    if (new_rrn < 0) {
      free(return_key);
      free(new_root);
      return BTREE_ERROR_IO;
    }

    new_root->rrn = new_rrn;
    b->root = new_root;

    if (write_root_rrn(b, b->root->rrn) != 0 ||
        write_index_record(b->io, b->root) != 0) {
      free(return_key);
      return BTREE_ERROR_IO;
    }
  }

  free(return_key);
  return status;
}

btree_status insert_in_page(page *p, key k, page *r_child, int pos) {
  if (!p) {
    puts("!!Error: page NULL");
    return BTREE_ERROR_INVALID_PAGE;
  }
  for (int i = p->child_number - 1; i >= pos; i--) {
    p->keys[i + 1] = p->keys[i];
  }

  p->keys[pos] = k;
  for (int i = p->child_number; i >= pos + 1; i--) {
    p->children[i + 1] = p->children[i];
  }

  if (r_child) {
    p->children[pos + 1] = r_child->rrn;
  }

  p->child_number += 1;
  return BTREE_INSERTED_IN_PAGE;
}

btree_status split(b_tree_buf *b, page *p, key k, page *r_child, key *promo_key,
                   page *new_page, int pos) {
  if (!b || !p || !promo_key || !new_page || pos < 0 || pos >= ORDER) {
    return BTREE_ERROR_INVALID_PAGE;
  }

  int m = (ORDER - 1) / 2;
  key temp_keys[ORDER];
  int temp_children[ORDER + 1];

  memset(temp_keys, 0, sizeof(temp_keys));
  memset(temp_children, 0, sizeof(temp_children));

  for (int i = 0, j = 0; i < p->child_number && j < ORDER; i++, j++) {
    if (j == pos) {
      if (j < ORDER) {
        temp_keys[j] = k;
        if (j + 1 < ORDER + 1)
          temp_children[j + 1] = r_child ? r_child->rrn : p->children[j + 1];
        j++;
      }
    }
    if (j < ORDER) {
      temp_keys[j] = p->keys[i];
      temp_children[j] = p->children[i];
    }
  }

  if (pos < ORDER + 1) {
    temp_children[pos + 1] = r_child ? r_child->rrn : p->children[pos + 1];
  }

  *promo_key = temp_keys[m];

  memset(p->keys, 0, sizeof(key) * ORDER);
  memset(p->children, 0, sizeof(int) * (ORDER + 1));

  p->child_number = m;
  for (int i = 0; i < m && i < ORDER; i++) {
    p->keys[i] = temp_keys[i];
    p->children[i] = temp_children[i];
  }
  if (m < ORDER + 1) {
    p->children[m] = temp_children[m];
  }

  new_page->child_number = ORDER - 1 - m;
  for (int i = 0; i < new_page->child_number && i < ORDER; i++) {
    new_page->keys[i] = temp_keys[m + 1 + i];
    new_page->children[i] = temp_children[m + 1 + i];
  }
  if (new_page->child_number < ORDER + 1) {
    new_page->children[new_page->child_number] = temp_children[ORDER];
  }

  int new_rrn = get_free_rrn(b->i);
  if (new_rrn < 0) {
    return BTREE_ERROR_IO;
  }
  new_page->rrn = new_rrn;

  if (write_index_record(b->io, p) < 0 ||
      write_index_record(b->io, new_page) < 0) {
    return BTREE_ERROR_IO;
  }

  return BTREE_SUCCESS;
}

btree_status insert_key(b_tree_buf *b, page *p, key k, key *promo_key,
                        page **r_child) {
  if (!b || !promo_key || !r_child) {
    return BTREE_ERROR_INVALID_PAGE;
  }

  page *temp = NULL;
  key promo;
  int flag, pos;

  if (!p) {
    *promo_key = k;
    *r_child = NULL;
    return BTREE_PROMOTION;
  }

  flag = search_in_page(p, k, &pos);
  if (flag == BTREE_FOUND_KEY) {
    return BTREE_ERROR_DUPLICATE;
  }

  if (!p->leaf) {
    puts("NOT LEAF! getting down!!!");
    temp = load_page(b, p->children[pos]);
    if (!temp)
      return BTREE_ERROR_IO;

    flag = insert_key(b, temp, k, &promo, r_child);

    if (temp != p)
      free(temp);

    if (flag != 0)
      return flag;
  }

  if (p->child_number < ORDER - 1)
    return insert_in_page(p, promo, *r_child, pos);

  page *new_page = alloc_page();
  if (!new_page) {
    return BTREE_ERROR_MEMORY;
  }

  key new_promo_key;
  btree_status split_status =
      split(b, p, promo, *r_child, &new_promo_key, new_page, pos);

  if (split_status != BTREE_SUCCESS) {
    free(new_page);
    return split_status;
  }

  *promo_key = new_promo_key;
  *r_child = new_page;
  return BTREE_PROMOTION;
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
  for (int i = 0; i < page->child_number; i++) {
    printf("key id: %s | data_rrn: %hu;\t", page->keys[i].id,
           page->keys[i].data_register_rrn);
  }
  printf("\n");
  printf("children rrn: ");
  for (int i = 0; i < page->child_number + 1; i++) {
    printf("i:%d= %hu\t", i, page->children[i]);
  }
  printf("\n");
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

int write_index_record(io_buf *io, page *p) {
  if (!io || !p) {
    puts("!!Error: invalid parameters");
    return BTREE_ERROR_IO;
  }

  if (!io->br || !io->fp) {
    puts("!!Error: invalid buffer or file pointer");
    return BTREE_ERROR_MEMORY;
  }

  print_page(p);
  int byte_offset = (io->br->header_size) + ((io->br->page_size) * p->rrn);
  printf("byte_offset %d\n", byte_offset);

  if (fseek(io->fp, byte_offset, SEEK_SET) != 0) {
    puts("!!Error: seek operation failed");
    return BTREE_ERROR_IO;
  }

  if (fwrite(p, io->br->page_size, 1, io->fp) != 1) {
    puts("!!Error: write operation failed");
    return BTREE_ERROR_IO;
  }

  fflush(io->fp);
  if (DEBUG) {
    printf("@Successfully wrote page %hu at offset %d\n", p->rrn, byte_offset);
  }
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

  populate_tree_header(io->br, io->br->free_rrn_address);
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
    p->child_number = 0;
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
  puts("error while freeing page");
}
