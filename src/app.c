#include "app.h"
#include "io-buf.h"
#include "b-tree.h"
#include "queue.h"
#include "page.h"

void cli() {
  puts("cli");
  return;
}

app* alloc_app() {
  app* a = malloc(sizeof(app));
  a->in = alloc_io_buf();
  a->out = alloc_io_buf();
  a->b = alloc_tree_buf();
  a->b->root = NULL;
  if (a && a->in && a->out){
    if(DEBUG)
      puts("@Allocated APP_BUFFER");
    return a;
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
  if(app->b) {
    clear_tree_buf(app->b);
    app->b = NULL;
  }
  if(app->b->root) {
    free(app->b->root);
    app->b->root = NULL;
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

  char index_file[4096];
  char value;
  strcpy(index_file, "public/btree-");
  value = ORDER + '0';
  index_file[13] = value;
  strcat(index_file, ".idx");
  char data_file[MAX_ADDRESS];
  strcpy(data_file, "public/veiculos.dat");

  create_index_file(app->in, index_file);
  create_data_file(app->out, data_file);

  load_file(app->in, index_file, "index");
  load_file(app->out, data_file, "data");
  create_new_tree(app->b, app->out, app->in, 100);

  cli(); // TODO

  clear_app(app);
  return 0;
}
