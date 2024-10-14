#ifndef _QUEUE
#define _QUEUE

#include "defines.h"

// queue max
#define P 20

queue *alloc_queue();

void clear_queue(queue *queue);

void print_queue(queue *queue);

void push_page(queue *queue, page *page);

page *pop_page(queue *queue);

page *queue_search(queue *queue, u16 rrn);

#endif
