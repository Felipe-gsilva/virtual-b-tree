#include "b-tree.h"

b_tree *alloc_tree_buf() {
    b_tree *b = malloc(sizeof(b_tree));
    b->bh = malloc(sizeof(b_tree_header));
    b->io = alloc_io_buf();
    b->q = alloc_queue();
    if(b) {
        if(DEBUG)
            puts("@Allocated B_TREE_BUFFER");
        return b;
    }
    puts("!!Could not allocate B_TREE_BUFFER");
    return NULL;
}

void clear_tree_buf(b_tree *b) {
    if(b) {
        clear_io_buf(b->io);
        clear_queue(b->q);
        if(b->bh){
            free(b->bh);
            b->bh = NULL;
        }
        free(b);
        b = NULL;
    }
    if(DEBUG)
        puts("@B_TREE_BUFFER cleared");
}

void populate_tree_header(b_tree_header *bh) {
    if (bh == NULL) {
        puts("!!Header pointer is NULL, cannot populate");
        return;
    }

    bh->page_size = sizeof(page);
    bh->root_rrn = 0;
}

b_tree *create_new_tree(io_buf *io) {
    

    return NULL;
}

u16 search_key(page *page, key key, int *return_pos) {
    if(!page)
        return ERROR;

    for(int i = 0; i < ORDER-1; i++) {
        if(page->keys[i].key == key.key) {
            *return_pos = i;
            return FOUND;
        }
    }
    *return_pos = NOT_FOUND;
    return NOT_FOUND;
}

page *load_page(b_tree *b, u16 rrn) {
    if(!b->bh || !b->io) {
        puts("!!Error while loading page");
        return NULL;
    }
    
    // if is loaded on queue, just grab it from there
    page *q_page = queue_search(b->q, rrn);
    if(q_page != NULL){
        puts("@Page found on queue");
        return q_page;
    }

    int byte_offset = (rrn * b->bh->page_size) + sizeof(b_tree_header);
    fseek(b->io->fp, byte_offset, SEEK_SET);

    page *page = alloc_page();
    if(page)
        fread(page, b->bh->page_size ,1, b->io->fp);
    return page;
}

void driver(){}

u16 search(b_tree *b, u16 rrn, key key, u16 *found_rrn, u16 *found_pos, page *return_page) {
    page *root = load_page(b, rrn);
    if (root == NULL) {
        puts("!!Error: NULL root");
        return ERROR;
    }       

    page *page = load_page(b, rrn); 
    int pos;

    int flag = search_key(page, key, &pos);
    if(flag == FOUND) { // se b está nessa pagina, POS = FOUND, se tiver na esquerda, ou n existir, POS = NOT_FOUND
        *found_rrn = rrn;
        *found_pos = pos;
        *return_page = *page;
        return FOUND;
    }
    return search(b, page->children[pos], key, found_rrn, found_pos, return_page);
    return NOT_FOUND;
}

void driver();

bool insert_page(b_tree *b, page *page) {
    return false;
    // TODO
}

page* split(page *page);

bool promote();

page *redistribute();

bool remove_page(b_tree *b, page *page) {
    return false;
    // TODO
}


FILE *create_tree_file(char *file_name) {
    return NULL;
}

void print_register(b_tree_header *bp, u16 rrn) {
    
}
