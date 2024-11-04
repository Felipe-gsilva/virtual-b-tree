#include "app.h"
#include "../test/test.h"
#include "b-tree-buf.h"
#include "free-rrn-list.h"
#include "io-buf.h"
#include "queue.h"

void print_ascii_art(void) {
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
  u16 pos;
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
    printf("7. Print root\n");

    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
    case 0:
      return;
    case 1:
      get_id(0, placa);
      p = b_search(a->b, placa, &pos);
      if (p) {
        print_page(p);
        d = load_data_record(a->data, p->keys[pos].data_register_rrn);
        print_data_record(d);
        break;
      }
      puts("Page not found!");
      break;
    case 2:
      get_id(0, placa);
      b_update(a->b, a->data, a->ld, placa);
      break;
    case 3:
      printf("Insira as informações do veículo:\n");
      printf("Placa:");
      scanf("%s", d->placa);
      printf("Modelo:");
      scanf("%s", d->modelo);
      printf("Marca:");
      scanf("%s", d->marca);
      printf("Ano:");
      scanf("%d", &(d->ano));
      printf("Categoria:");
      scanf("%s", d->categoria);
      printf("Quilometragem:");
      scanf("%d", &(d->quilometragem));
      printf("Status:");
      scanf("%s", d->status);
      b_insert(a->b, a->data, d, get_free_rrn(a->b->i));
      d_insert(a->data, d, a->ld, get_free_rrn(a->ld));
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
    case 7:
      print_page(a->b->root);
      break;
    default:
      printf("Invalid choice.\n");
      break;
    }
  }
}

app *alloc_app(void) {
  app *a = malloc(sizeof(app));
  a->idx = alloc_io_buf();
  a->data = alloc_io_buf();
  a->b = alloc_tree_buf();
  a->ld = alloc_ilist();
  if (a->idx && a->data) {
    if (DEBUG)
      puts("@Allocated APP_BUFFER");
    return a;
  }

  puts("!!Error while allocating APP_BUFFER");
  return NULL;
}

void clear_app(app *app) {
  if (!DEBUG)
    puts("See you soon!!");
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
  if (app->ld) {
    clear_ilist(app->ld);
    app->ld = NULL;
  }
  if (app) {
    free(app);
    app = NULL;
  }
  if (app != NULL)
    puts("!! Error while clearing app");
}

int main(int argc, char **argv) {
  int n = 99;

  app *a;
  char *index_file = malloc(MAX_ADDRESS);
  char *data_file = malloc(MAX_ADDRESS);
  char converted_char;

  a = alloc_app();

  strcpy(index_file, "public/btree-");
  converted_char = ORDER + '0';
  index_file[strlen(index_file)] = converted_char;
  strcat(index_file, ".idx");
  strcpy(data_file, "public/veiculos.dat");

  create_index_file(a->b->io, index_file);
  create_data_file(a->data, data_file);

  load_file(a->b->io, index_file, "index");
  load_file(a->data, data_file, "data");

  free(data_file);
  free(index_file);

  load_list(a->b->i, a->b->io->br->free_rrn_address);
  load_list(a->ld, a->data->hr->free_rrn_address);

  page *temp = load_page(a->b, a->b->io->br->root_rrn);
  a->b->root = temp;
  if (ftell(a->b->io->fp) <= a->b->io->br->header_size) {
    insert_list(a->b->i, 0);
    build_tree(a->b, a->data, n);
    if (DEBUG) {
      print_queue(a->b->q);
      test_tree(a->b, a->data, n);
    }
    insert_list(a->ld, n + 1);
    a->b->io->br->root_rrn = a->b->root->rrn;
    write_index_header(a->b->io);
  }

  cli(a);

  if (DEBUG)
    test_queue_search();
  clear_app(a);
  return 0;
}
