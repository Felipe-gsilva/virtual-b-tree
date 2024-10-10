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

b_tree *create_new_tree() {
    return NULL;
}

u16 search_key(page *page, key key) {
    return 0;
}

page *load_page(b_tree *b, u16 rrn) {
    if(!b->bh || !b->io) {
        puts("!!Error while loading page");
        return NULL;
    }
    
    // if is loaded on queue, just grab it from there
    //  

    int byte_offset = rrn * b->bh->page_size + sizeof(b_tree_header);
    fseek(b->io->fp, byte_offset, SEEK_SET);

    page *page = alloc_page();
    if(page)
        fread(page, b->bh->page_size ,1, b->io->fp);
    return page;
}

void driver(){}

u16 search(b_tree *b, u16 rrn, key key, u16 found_rrn, u16 found_pos, page *return_page) {
    page *root = load_page(b, rrn);
    if (root == NULL) {
        puts("!!Error: NULL root");
        return ERROR;
    }       

    page *page = load_page(b, rrn); 

    u16 pos = search_key(page, key);
    if(pos == FOUND) { // se b está nessa pagina, POS = FOUND, se tiver na esquerda, ou n existir, POS = NOT_FOUND
        found_rrn = rrn;
        found_pos = pos;
        return_page = page;
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


FILE *create_tree_file(char *file_name);

void print_tree(b_tree_header *bp) {
    
}
