#include "queue.h"
#include "b-tree-buf.h"

queue *alloc_queue() {
    queue* root = malloc(sizeof(queue));
    if (!root) {
        puts("!!Error: Memory allocation failed");
        return NULL;
    }
    root->next = NULL;
    root->page = NULL;
    root->counter = 0;
    if(DEBUG)
        puts("@Allocated queue");
    return root;
}

void clear_queue(queue *queue) {
    if(!queue) {
        puts("!!Error: NULL queue pointer");
        return;
    }

    struct queue *current = queue->next;
    struct queue *next_node;

    while (current) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    if(current != NULL)
        free(current);
    queue->next = NULL; 
    if(DEBUG)
        puts("@Queue cleared");
}

void print_queue(queue *queue) {
    if(!queue) {
        puts("!!Error: NULL queue pointer");
        return;
    }   

    if(!queue->next) {
        puts("!!Error: Empty queue");
        return;
    }

    printf("Queue: ");
    struct queue *aux = queue->next;
    while (aux) {
        printf("%s ", aux->page->keys->id);
        aux = aux->next;
    }
    printf("\n");
}

void push_page(b_tree_buf *b, page *page) {
    if(!b->q) {
        puts("!!Error: NULL queue pointer");
        return;
    }

    struct queue *new_node = alloc_queue();
    if (!new_node) {
        puts("!!Error: Memory allocation failed");
        return;
    }

    if(b->q->counter >= P) 
        pop_page(b);

    new_node->page = page;
    new_node->next = NULL;
    struct queue *temp = b->q;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new_node;
    b->q->counter++;

    if(DEBUG)
        puts("@Pushed on queue");
}

page *pop_page(b_tree_buf *b) {
    if(!b->q) {
        puts("!!Error: NULL or Empty queue pointer");
        return NULL;
    }

    struct queue *free_q = b->q->next;
    page *page = free_q->page;

    b->q->next = free_q->next;

    free(free_q);
    b->q->counter--;
    if(DEBUG)
        puts("@Popped from queue");
    write_index_record(b->io, page);
    return page;
}

page *queue_search(queue *queue, u16 rrn) {
    if(!queue) {
        puts("!!Error: NULL or Empty queue pointer");
        return NULL;
    }

    struct queue *temp = queue;
    while (temp->next != NULL && temp->page->rrn != rrn) {
        temp = temp->next;
    }

    if(temp) {
        if(DEBUG)
            puts("@Found on queue");
        return temp->page;
    }
    puts("!!Error: page not found on queue");
    return NULL;
}
