#ifndef _FREE_RRN_LIST
#define _FREE_RRN_LIST
#include "defines.h"

i_list *alloc_ilist();

void clear_ilist(i_list *i);

void load_list(i_list *i, char* s);

u16 get_free_rrn(i_list *i);

u16 *load_rrns(i_list *i);

void i_insert(i_list *i, u16 rrn); // TODO

#endif
