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
    puts("!!Error: NULL queue pointer\n");
    return;
  }
  queue *current = q->next;
  queue *next_node;
  while (current) {
    next_node = current->next;
    if (current->page) {
      clear_page(current->page);
      current->page = NULL;
    }
    free(current);
    current = next_node;
  }
  // Do not attempt to access q->page if it's a dummy head
  q->next = NULL;
  q->counter = 0;

  if (DEBUG) {
    puts("@Queue cleared");
  }
}

void print_queue(queue *q) {
  if (!q) {
    fprintf(stderr, "!!Error: NULL queue pointer\n");
    return;
  }

  if (!q->next) {
    fprintf(stderr, "!!Error: Empty queue\n");
    return;
  }

  printf("Queue contents:\n");
  queue *current = q->next;
  int node_count = 0;

  while (current) {
    if (!current->page) {
      fprintf(stderr, "!!Error: NULL page in queue node %d\n", node_count);
      current = current->next;
      continue;
    }

    printf("Node %d (RRN: %d) Keys: ", node_count, current->page->rrn);

    for (int i = 0; i < current->page->keys_num && i < ORDER - 1; i++) {
      if (current->page->keys[i].id[0] != '\0') {
        printf("%s ", current->page->keys[i].id);
      }
    }
    printf("\n");

    current = current->next;
    node_count++;
  }

  if (DEBUG) {
    printf("Total nodes in queue: %d\n", node_count);
  }
}

void push_page(b_tree_buf *b, page *p) {
  if (!b->q || !p) {
    puts("!!Error: NULL queue pointer or page");
    return;
  }

  // Use updated queue_search
  page *temp_page = queue_search(b->q, p->rrn);
  if (temp_page != NULL) {
    if (DEBUG)
      puts("@Page already found in queue");
    memcpy(temp_page, p, sizeof(page));
    return;
  }

  // Proceed to add the new page to the queue
  if (b->q->counter >= P) {
    page *popped_page = pop_page(b);
    if (DEBUG && popped_page) {
      printf("@Popped page with RRN %hu from queue\n", popped_page->rrn);
    }
  }

  queue *new_node = alloc_queue();
  if (!new_node) {
    puts("!!Error: Memory allocation failed");
    return;
  }

  new_node->page = p;
  new_node->next = NULL;

  // Insert the new node at the end of the queue
  queue *temp = b->q;
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = new_node;
  b->q->counter++;
  if (DEBUG)
    puts("@Pushed page onto queue");
}

page *pop_page(b_tree_buf *b) {
  if (!b->q || b->q->next == NULL) {
    puts("!!Error: NULL or Empty queue pointer");
    return NULL;
  }

  queue *head = b->q->next;
  page *page = head->page;

  b->q->next = head->next;
  b->q->counter--;

  if (DEBUG)
    puts("@Popped from queue");

  free(head);
  return page;
}

page *queue_search(queue *q, u16 rrn) {
  if (!q) {
    if (DEBUG)
      puts("!!Error: NULL or Empty queue pointer");
    return NULL;
  }
  queue *temp = q->next; // Start after the dummy head
  while (temp != NULL) {
    if (temp->page && temp->page->rrn == rrn) {
      if (DEBUG)
        puts("@Page found in queue");
      return temp->page;
    }
    temp = temp->next;
  }
  if (DEBUG)
    puts("@Page not found in queue");
  return NULL;
}
