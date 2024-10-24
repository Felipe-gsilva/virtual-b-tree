#include "free_rrn_list.h"
#include "io-buf.h"

i_list *alloc_ilist() {
  i_list *i = malloc(sizeof(i_list));
  i->io = alloc_io_buf();
  if(i)
    return i;
  exit(-1);
}

void clear_ilist(i_list *i) {
  if(i) {
    free(i->free_rrn);
    i->free_rrn = NULL;
    clear_io_buf(i->io);
    free(i); 
    i = NULL;
  }
  if(DEBUG)
    puts("@list cleared");
}

void load_list(i_list *i, char* s) {
  if(!i) {
    puts("!!Error: empty i list");
    return;
  }

  if(!i->io) {
    puts("!!Error: empty io buffer on i list");
    return;
  }
  printf("@Opened rrn helper %s\n", s);

  strcpy(i->io->address, s);
  i->io->fp = fopen(i->io->address, "r+b");
  if (!i->io->fp) {
    printf("!!Error opening file: %s", s);
    i->io->fp = fopen(i->io->address, "wb");
    if(i->io->fp) {
      fclose(i->io->fp);
      i->io->fp = fopen(i->io->address, "r+b");
    }
  }
}

u16 *load_rrn_list(i_list *i) {
  u16 *c;
  c = malloc(sizeof(u16) * i->n);

  fseek(i->io->fp, 0, SEEK_SET);
  int flag = fread(c, sizeof(c), i->n, i->io->fp);
  if(!flag)
    return c;

  puts("!!Error: no free rrn on i_list");
  return NULL;
}

u16 get_free_rrn(i_list *i) {
  i->free_rrn = load_rrn_list(i);
  if(i->free_rrn)
    return i->free_rrn[0];
  puts("!!Error: null free rrn pointer");
  exit(0);
}

void insert_list(i_list *i, u16 rrn) {
  if(!i->io->fp) {
    puts("!!Error: NULL rrn list");
    return;
  }
  
  fseek(i->io->fp, i->n * sizeof(u16), SEEK_SET);
  fwrite(&rrn, sizeof(u16), 1, i->io->fp);
  i->n++;
}
