#ifndef _PAGE_H
#define _PAGE_H

#include "defines.h"

page *alloc_page();

page *new_page(u16 rrn, key keys[], u16 children[]);

void clear_page(page *page);

#endif 
