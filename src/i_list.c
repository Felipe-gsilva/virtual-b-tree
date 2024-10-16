#include "i_list.h"
#include "io-buf.h"

i_list *alloc_ilist() {
  i_list *i = malloc(sizeof(i_list));
  i->in = alloc_io_buf();
  i->out = alloc_io_buf();
  if(i)
    return i;
  exit(-1);
}

void clear_ilist(i_list *i) {
  if(i) {
    clear_io_buf(i->out);
    clear_io_buf(i->in);
    free(i); 
    i = NULL;
  }
  if(DEBUG)
    puts("@i_list cleared");
}

void load_list(i_list *i, char* s) {
  if(!i) {
    puts("!!Error: empty i list");
    return;
  }
  if(!i->in) {
    puts("!!Error: empty io buffer on i list");
    return;
  }

  i->in->fp = fopen(s, "w");
  if (!i->in->fp) {
    printf("!!Error opening file: %s", s);
    return; 
  }
  fprintf(i->in->fp, "FREE RRN LIST");
}
