#include "app.h"
#include "io-buf.h"
#include "b-tree.h"

void print_ascii_art() {
  printf("                                         ,----,                                \n");
  printf("                                       ,/   .`|                                \n");
  printf("                ,---,.               ,`   .'  :,-.----.       ,---,.    ,---,. \n");
  printf("       ,---.  ,'  .'  \\            ;    ;     /\\    /  \\    ,'  .' |  ,'  .' | \n");
  printf("      /__./|,---.' .' |    ,---,..'___,/    ,' ;   :    \\ ,---.'   |,---.'   | \n");
  printf(" ,---.;  ; ||   |  |: |  ,'  .' ||    :     |  |   | .\\ : |   |   .'|   |   .' \n");
  printf("/___/ \\  | |:   :  :  /,---.'   ,;    |.';  ;  .   : |: | :   :  |-,:   :  |-, \n");
  printf("\\   ;  \\ ' |:   |    ; |   |    |`----'  |  |  |   |  \\ : :   |  ;/|:   |  ;/| \n");
  printf(" \\   \\  \\: ||   :     \\:   :  .'     '   :  ;  |   : .  / |   :   .'|   :   .' \n");
  printf("  ;   \\  ' .|   |   . |:   |.'       |   |  '  ;   | |  \\ |   |  |-,|   |  |-, \n");
  printf("   \\   \\   ''   :  '; |`---'         '   :  |  |   | ;\\  \\'   :  ;/|'   :  ;/| \n");
  printf("    \\   `  ;|   |  | ;               ;   |.'   :   ' | \\.|   |    \\|   |    \\ \n");
  printf("     :   \\ ||   :   /                '---'     :   : :-'  |   :   .'|   :   .' \n");
  printf("      '---\" |   | ,'                           |   |.'    |   | ,'  |   | ,'   \n");
  printf("            `----'                             `---'      `----'    `----'     \n");
  printf("                                                                                \n");
}

void cli(app *a) {
  // TODO
  int choice = -1;
  u8 id;
  char placa[TAMANHO_PLACA];

  print_ascii_art();

  while(choice != 0) {
    printf("Choose an option:\n");
    printf("0. Exit\n");
    printf("1. List\n");
    printf("2. Update\n");
    printf("3. Insert\n");
    printf("4. Remove\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
      case 0:
        return;
      case 1:
        //print_tree();
        break;
      case 2:
        printf("Enter ID to update: ");
        scanf("%s", placa);
        printf("%s", placa);
        //update_key(id);
        break;
      case 3:
        //insert_key();
        break;
      case 4:
        printf("Enter ID to remove: ");
        scanf("%s", placa);
        puts("tamanho paia");
        printf("%s", placa);
        //remove_key(id);
        break;
      default: 
        printf("Invalid choice.\n");
        break;
    }
  }
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
  if(app) {
    free(app);
    app = NULL;
  }
  if(app)
    puts("!! Error while clearing app");
}

int main(int argc, char **argv) {
  app *a;
  a = alloc_app();

  // ----
  char index_file[4096];
  char value;
  strcpy(index_file, "public/btree-");
  value = ORDER + '0';
  index_file[13] = value;
  strcat(index_file, ".idx");
  char data_file[MAX_ADDRESS];
  strcpy(data_file, "public/veiculos.dat");
  // ----

  create_index_file(a->in, index_file);
  create_data_file(a->out, data_file);

  load_file(a->in, index_file, "index");
  load_file(a->out, data_file, "data");
  create_new_tree(a->b, a->out, a->in, 100);

  cli(a); 

  clear_app(a);
  return 0;
}
