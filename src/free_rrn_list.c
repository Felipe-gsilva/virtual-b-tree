#include "free_rrn_list.h"
#include "io-buf.h"

void sort_list(u16 A[], int n) {
  int h = 1;
  while (h < n / 3) {
    h = 3 * h + 1;
  }

  while (h >= 1) {
    for (int i = h; i < n; i++) {
      int aux = A[i];
      int j = i;
      while (j >= h && A[j - h] > aux) {
        A[j] = A[j - h];
        j -= h;
      }
      A[j] = aux;
    }
    h = h / 3;
  }

  for (int i = 0; i < n; i++)
    printf("lista fera: %hu: ", A[i]);
}

int rrn_exists(u16 A[], int n, u16 rrn) {
  for (int i = 0; i < n; i++) {
    if (A[i] == rrn)
      return 1;
  }
  return 0;
}

void write_rrn_list_to_file(free_rrn_list *i) {
  if (!i || !i->io->fp)
    return;

  fseek(i->io->fp, 0, SEEK_SET);
  if (fwrite(&i->n, sizeof(u16), 1, i->io->fp) != 1) {
    puts("!!Error: Failed to write RRN count");
    return;
  }

  if (i->n > 0) {
    fseek(i->io->fp, sizeof(u16), SEEK_SET);
    if (fwrite(i->free_rrn, sizeof(u16), i->n, i->io->fp) != i->n) {
      puts("!!Error: Failed to write RRN list");
    }
  }
}

free_rrn_list *alloc_ilist() {
  free_rrn_list *i = malloc(sizeof(free_rrn_list));
  if (!i)
    exit(-1);
  i->io = alloc_io_buf();
  i->n = 0;
  i->free_rrn = NULL;
  return i;
}

void clear_ilist(free_rrn_list *i) {
  if (!i)
    return;

  if (i->io && i->io->fp) {
    fclose(i->io->fp);
    i->io->fp = NULL;
  }

  free(i->free_rrn);
  i->free_rrn = NULL;

  clear_io_buf(i->io);
  free(i);
}

void load_list(free_rrn_list *i, char *s) {
  if (!i || !s) {
    puts("!!Error: Invalid parameters");
    return;
  }

  if (i->free_rrn) {
    free(i->free_rrn);
    i->free_rrn = NULL;
  }

  if (!i->io) {
    i->io = alloc_io_buf();
    if (!i->io) {
      puts("!!Error: Failed to allocate IO buffer");
      return;
    }
  }

  strncpy(i->io->address, s, (strlen(s)));
  i->io->address[strlen(s)] = '\0';

  i->io->fp = fopen(i->io->address, "r+b");
  if (!i->io->fp) {
    printf("Creating new RRN list file: %s\n", s);
    i->io->fp = fopen(i->io->address, "wb");
    if (!i->io->fp) {
      printf("!!Error: Cannot create file %s\n", s);
      return;
    }
    i->n = 0;
    fwrite(&i->n, sizeof(u16), 1, i->io->fp);
    fclose(i->io->fp);
    i->io->fp = fopen(i->io->address, "r+b");
  }

  fseek(i->io->fp, 0, SEEK_SET);
  size_t read = fread(&i->n, sizeof(u16), 1, i->io->fp);
  if (read != 1) {
    i->n = 0;
    fseek(i->io->fp, 0, SEEK_SET);
    fwrite(&i->n, sizeof(u16), 1, i->io->fp);
  }

  if (i->n > 0) {
    i->free_rrn = load_rrn_list(i);
    if (!i->free_rrn) {
      i->n = 0;
      fseek(i->io->fp, 0, SEEK_SET);
      fwrite(&i->n, sizeof(u16), 1, i->io->fp);
    }
  }

  printf("@Loaded RRN list with %d entries\n", i->n);
}

u16 *load_rrn_list(free_rrn_list *i) {
  if (!i->io->fp || i->n == 0)
    return NULL;

  u16 *list = malloc(sizeof(u16) * i->n);
  if (!list) {
    puts("!!Error: memory allocation failed");
    return NULL;
  }

  fseek(i->io->fp, sizeof(u16), SEEK_SET);
  int read = fread(list, sizeof(u16) * i->n, 1, i->io->fp);

  if (read != 1) {
    free(list);
    printf("read thing %d\n", read);

    puts("!!Error: could not read complete RRN list");
    return NULL;
  }

  if (DEBUG) {
    for (int j = 0; j < i->n; j++)
      printf("i->list[%d]: %d\t", j, list[j]);
    puts("");
  }
  return list;
}

u16 get_free_rrn(free_rrn_list *i) {
  if (!i || !i->io->fp) {
    puts("!!Error: Invalid list or file pointer");
    exit(1);
  }

  free(i->free_rrn);
  i->free_rrn = load_rrn_list(i);

  if (!i->free_rrn || i->n == 0) {
    puts("!!Error: No free RRNs available; initializing with default");
    insert_list(i, 0);
    return 0;
  }

  u16 rrn = i->free_rrn[0];
  i->n--;

  memmove(i->free_rrn, i->free_rrn + 1, sizeof(u16) * i->n);

  write_rrn_list_to_file(i);

  return rrn;
}

u16 get_last_free_rrn(free_rrn_list *i) {
  if (!i || !i->io->fp) {
    puts("!!Error: invalid list or file pointer");
    return (u16)-1;
  }

  if (!i->free_rrn)
    i->free_rrn = load_rrn_list(i);

  if (!i->free_rrn || i->n == 0) {
    puts("!!Error: no free RRNs available");
    return (u16)-1;
  }
  return i->free_rrn[i->n - 1];
}

void insert_list(free_rrn_list *i, u16 rrn) {
  if (!i || !i->io->fp) {
    puts("!!Error: NULL rrn list or file pointer");
    return;
  }

  if (rrn_exists(i->free_rrn, i->n, rrn)) {
    printf("@RRN %d already exists in the list\n", rrn);
    return;
  }

  u16 *new_list = realloc(i->free_rrn, (i->n + 1) * sizeof(u16));
  if (!new_list) {
    puts("!!Error: Memory allocation failed");
    return;
  }

  i->free_rrn = new_list;
  i->free_rrn[i->n++] = rrn;

  sort_list(i->free_rrn, i->n);
  write_rrn_list_to_file(i);

  if (DEBUG) {
    printf("@RRN %d added and list sorted. New list:\n", rrn);
    for (int j = 0; j < i->n; j++) printf("%d ", i->free_rrn[j]);
    puts("");
  }
}
