#ifndef _queue
#define _queue

#include "page.h"

// queue max
#define P 20

typedef struct queue queue;

struct queue {
    queue *next;
    page *page;
    u8 counter;
};

queue *alloc_queue();

void clear_queue(queue *queue);

void print_queue(queue *queue);

void push_page(queue *queue, page *page);

page *pop_page(queue *queue);

#endif
