#include "app.h"

void cli() {
    puts("cli");
    return;
}

app* alloc_app() {
    app* appl = malloc(sizeof(app));
    appl->in = alloc_io_buf();
    appl->out = alloc_io_buf();
    if (appl && appl->in && appl->out){
        puts("@Allocated APP_BUFFER\n");
        return appl;
    }
        
    puts("!!Error while allocating APP_BUFFER\n");
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

    // test --------

    //load_file(app->in, "public/data.ct");
    // ------

    char file_name[4096];
    strcpy(file_name, "public/btree-");
    char value;
    value = ORDER+'0';
    file_name[13] = value;
    strcat(file_name, ".idx");

    app->out->fp = create_tree_file(file_name);
    if (app->out->fp == NULL) {
        printf("Failed to create b-tree\n");
        exit(0);
    }

    clear_app(app);
    return 0;
}
