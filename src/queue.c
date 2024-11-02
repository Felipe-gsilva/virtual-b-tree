#include "queue.h"
#include "b-tree-buf.h"

queue *alloc_queue() {
  queue *root = malloc(sizeof(queue));
  if (!root) {
    puts("!!Error: Memory allocation failed");
    return NULL;
  }
  root->next = NULL;
  root->page = NULL;
  root->counter = 0;
  if (DEBUG)
    puts("@Allocated queue");
  return root;
}

void clear_queue(queue *q) {
  if (!q) {
    puts("!!Error: NULL queue pointer");
    return;
  }

  queue *current = q->next;
  queue *next_node;

  while (current) {
    next_node = current->next;
    free(current);
    current = next_node;
  }
  if (current != NULL)
    free(current);
  q->next = NULL;
  if (DEBUG)
    puts("@Queue cleared");
}

void print_queue(queue *q) {
  if (!q) {
    puts("!!Error: NULL queue pointer");
    return;
  }

  if (!q->next) {
    puts("!!Error: Empty queue");
    return;
  }

  printf("Queue: ");
  queue *aux = q->next;
  while (aux) {
    for (int i = 0; i < aux->page->child_num; i++)
      printf("%s \t", aux->page->keys[i].id);
    aux = aux->next;
  }
  puts("");
}

void push_page(b_tree_buf *b, page *p) {
  if (!b->q) {
    puts("!!Error: NULL queue pointer");
    return;
  }

  page* temp_page = queue_search(b->q, p->rrn);
  if (temp_page != NULL) {
    if (DEBUG)
      puts("@Page already found");
    p->rrn = temp_page->rrn;
    memcpy(p->keys, temp_page->keys, sizeof(temp_page->keys));
    p->leaf = temp_page->leaf;
    memcpy(p->children, temp_page->children, sizeof(temp_page->children));
    p->child_num= temp_page->child_num;
    if(p)
      return;

    exit(0);
  }

  queue *new_node = alloc_queue();
  if (!new_node) {
    puts("!!Error: Memory allocation failed");
    return;
  }

  if (b->q->counter >= P)
    pop_page(b);

  new_node->page = p;
  new_node->next = NULL;
  queue *temp = b->q;
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = new_node;
  b->q->counter++;

  if (DEBUG)
    puts("@Pushed on queue");
}

page *pop_page(b_tree_buf *b) {
  if (!b->q) {
    puts("!!Error: NULL or Empty queue pointer");
    return NULL;
  }

  queue *q = b->q->next;
  page *page = q->page;

  b->q->next = q->next;

  free(q);
  b->q->counter--;
  if (DEBUG)
    puts("@Popped from queue");
  write_index_record(b->io, page);
  return page;
}

page *queue_search(queue *q, u16 rrn) {
  if (!q) {
    puts("!!Error: NULL or Empty queue pointer");
    return NULL;
  }

  queue *temp = q;
  while (temp != NULL) {
    if (temp->page && temp->page->rrn == rrn) {
      if (DEBUG)
        puts("@Found on queue");
      return temp->page;
    }
    temp = temp->next;
  }

  puts("!!Error: page not found on queue");
  return NULL;
}
