#ifndef _I_LIST
#define _I_LIST
#include "defines.h"

i_list *alloc_ilist();

void clear_ilist(i_list *i);

void load_list(i_list *i, char* s);

u16 get_free_rrn(i_list *i);

u16 *load_rrn_list(i_list *i);

#endif
