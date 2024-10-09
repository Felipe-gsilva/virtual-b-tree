#include "app.h"

void cli() {
    puts("cli");
    return;
}

app* alloc_app() {
    app* appl = malloc(sizeof(app));
    appl->in = alloc_io_buf();
    appl->out = alloc_io_buf();
    appl->queue = alloc_queue();
    if (appl && appl->in && appl->out){
        puts("@Allocated APP_BUFFER");
        return appl;
    }
        
    puts("!!Error while allocating APP_BUFFER");
    return NULL;
}

void clear_app(app* app) {
    if(app->in) {
        clear_io_buf(app->in);   
        app->in = NULL;
    }
    if(app->out) {
        clear_io_buf(app->out);
        app->out = NULL;
    }
    if(app->queue){
        clear_queue(app->queue);
        app->queue =NULL;
    }
    if(app) {
        free(app);
        app = NULL;
    }
    if(app)
        puts("!! Error while clearing app");
}

int main(int argc, char **argv) {
    app *app;
    app = alloc_app();

    cli();

    char file_name[4096];
    strcpy(file_name, "public/btree-");
    char value;
    value = ORDER+'0';
    file_name[13] = value;
    strcat(file_name, ".idx");

    app->out->fp = create_data_file(file_name); 
    if (app->out->fp == NULL) {
        printf("Failed to create b-tree\n");
        exit(0);
    }


    key keys[ORDER-1];
    keys[0].key = 2;
    
    push_page(app->queue, new_page(0,keys,0));
    push_page(app->queue, new_page(1,0,0));
    push_page(app->queue, new_page(2,0,0));
    push_page(app->queue, new_page(3,0,0));
    push_page(app->queue, new_page(4,0,0));
    push_page(app->queue, new_page(5,0,0));
    push_page(app->queue, new_page(6,0,0));
    push_page(app->queue, new_page(5,0,0));
    push_page(app->queue, new_page(5,0,0));
    push_page(app->queue, new_page(5,0,0));
    push_page(app->queue, new_page(5,0,0));
    push_page(app->queue, new_page(5,0,0));

    printf("counter: %hu\n", app->queue->counter);
    print_queue(app->queue);
    
    clear_queue(app->queue);
    print_queue(app->queue);

    push_page(app->queue, new_page(1,0,0));
    print_queue(app->queue);

    clear_app(app);
    return 0;
}