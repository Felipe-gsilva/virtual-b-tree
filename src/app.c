#include "app.h"
#include "b-tree-buf.h"
#include "free-rrn-list.h"
#include "io-buf.h"
#include "queue.h"

void print_ascii_art() {
  printf("                                         ,----,                      "
         "          \n");
  printf("                                       ,/   .`|                      "
         "          \n");
  printf("                ,---,.               ,`   .'  :,-.----.       ,---,. "
         "   ,---,. \n");
  printf("       ,---.  ,'  .'  \\            ;    ;     /\\    /  \\    ,'  "
         ".' |  ,'  .' | \n");
  printf("      /__./|,---.' .' |    ,---,..'___,/    ,' ;   :    \\ ,---.'   "
         "|,---.'   | \n");
  printf(" ,---.;  ; ||   |  |: |  ,'  .' ||    :     |  |   | .\\ : |   |   "
         ".'|   |   .' \n");
  printf("/___/ \\  | |:   :  :  /,---.'   ,;    |.';  ;  .   : |: | :   :  "
         "|-,:   :  |-, \n");
  printf("\\   ;  \\ ' |:   |    ; |   |    |`----'  |  |  |   |  \\ : :   |  "
         ";/|:   |  ;/| \n");
  printf(" \\   \\  \\: ||   :     \\:   :  .'     '   :  ;  |   : .  / |   :  "
         " .'|   :   .' \n");
  printf("  ;   \\  ' .|   |   . |:   |.'       |   |  '  ;   | |  \\ |   |  "
         "|-,|   |  |-, \n");
  printf("   \\   \\   ''   :  '; |`---'         '   :  |  |   | ;\\  \\'   :  "
         ";/|'   :  ;/| \n");
  printf("    \\   `  ;|   |  | ;               ;   |.'   :   ' | \\.|   |    "
         "\\|   |    \\ \n");
  printf("     :   \\ ||   :   /                '---'     :   : :-'  |   :   "
         ".'|   :   .' \n");
  printf("      '---\" |   | ,'                           |   |.'    |   | ,'  "
         "|   | ,'   \n");
  printf("            `----'                             `---'      `----'    "
         "`----'     \n");
  printf("                                                                     "
         "           \n");
}

void get_id(int i, char *placa) {
  if (i >= 3) {
    puts("Many tries");
    return;
  }
  puts("Digite a placa");
  scanf("%s", placa);
  if (strlen(placa) != 7) {
    puts("!!Wrong size mate");
    get_id(i + 1, placa);
  }
}

void cli(app *a) {
  int choice = -1;
  page *p;
  data_record *d = malloc(sizeof(data_record));
  char placa[TAMANHO_PLACA];
  print_ascii_art();

  while (choice != 0) {
    printf("Choose an option:\n");
    printf("0. Exit\n");
    printf("1. Search by id\n");
    printf("2. Update\n");
    printf("3. Insert\n");
    printf("4. Remove\n");
    printf("5. Print Queue\n");
    printf("6. Clear Queue\n");
    
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
    case 0:
      return;
    case 1:
      get_id(0, placa);
      p = b_search(a->b, placa);
      if (p) {
        print_page(p);
        break;
      }
      puts("Page not found!");
      break;
    case 2:
      // b_update(id);
      break;
    case 3:
      b_insert(a->b, a->data, d, get_free_rrn(a->b->i));
      break;
    case 4:
      get_id(0, placa);
      b_remove(a->b, a->data, placa);
      break;
    case 5:
      print_queue(a->b->q);
      break;
    case 6:
      clear_queue(a->b->q);
      break;
    default:
      printf("Invalid choice.\n");
      break;
    }
  }
}

app *alloc_app() {
  app *a = malloc(sizeof(app));
  a->idx = alloc_io_buf();
  a->data = alloc_io_buf();
  a->b = alloc_tree_buf();
  if (a && a->idx && a->data) {
    if (DEBUG)
      puts("@Allocated APP_BUFFER");
    return a;
  }

  puts("!!Error while allocating APP_BUFFER");
  return NULL;
}

void clear_app(app *app) {
  if (app->idx) {
    clear_io_buf(app->idx);
    app->idx = NULL;
  }
  if (app->data) {
    clear_io_buf(app->data);
    app->data = NULL;
  }
  if (app->b) {
    clear_tree_buf(app->b);
    app->b = NULL;
  }
  if (app) {
    free(app);
    app = NULL;
  }
  if (app)
    puts("!! Error while clearing app");
}

int main(int argc, char **argv) {
  app *a;
  char *index_file = malloc(MAX_ADDRESS);
  char *data_file = malloc(MAX_ADDRESS);
  char converted_char;

  a = alloc_app();

  strcpy(index_file, "public/btree-");
  converted_char = ORDER + '0';
  index_file[13] = converted_char;
  strcat(index_file, ".idx");
  strcpy(data_file, "public/veiculos.dat");

  create_index_file(a->b->io, index_file);
  create_data_file(a->data, data_file);

  load_file(a->b->io, index_file, "index");
  load_file(a->data, data_file, "data");

  free(data_file);
  free(index_file);

  load_list(a->b->i, a->b->io->br->free_rrn_address);
  page *temp = load_page(a->b, a->b->io->br->root_rrn);
  if (ftell(a->b->io->fp) <= a->b->io->br->header_size) {
    insert_list(a->b->i, 0);
    build_tree(a->b, a->data, 98);
    print_queue(a->b->q);
    test_tree(a->b, a->data, 98);
  }

  cli(a);
  clear_app(a);
  return 0;
}
