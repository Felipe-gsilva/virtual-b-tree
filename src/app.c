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
    appl->b = alloc_tree_buf();
    if (appl && appl->in && appl->out){
        if(DEBUG)
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
    if(app->b) {
        clear_tree_buf(app->b);
        app->b = NULL;
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

    char index_file[4096];
    strcpy(index_file, "public/btree-");
    char value;
    value = ORDER+'0';
    index_file[13] = value;
    strcat(index_file, ".idx");

    create_data_file(app->out, index_file);
    if (app->out->fp == NULL) {
        printf("Failed to create b-tree\n");
        exit(0);
    }

    // -- test on queue
    //    key keys[ORDER-1];
    //    push_page(app->queue, new_page(0,keys,0));
    //
    //    printf("counter: %hu\n", app->queue->counter);
    //    print_queue(app->queue);
    //    
    //    clear_queue(app->queue);
    //    print_queue(app->queue);
    //
    //    push_page(app->queue, new_page(1,0,0));
    //    print_queue(app->queue);
    // ---------
    
    load_file(app->in, index_file);


    char data_file[MAX_ADDRESS];
    strcpy(data_file, "public/veiculos.dat");
    load_file(app->out, data_file);
    read_data_register(app->out, 0);

    clear_app(app);
    return 0;
}
