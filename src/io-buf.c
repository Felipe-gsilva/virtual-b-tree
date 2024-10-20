#include "io-buf.h"
#include "b-tree-buf.h"
#include "i_list.h"

io_buf *alloc_io_buf() {
  io_buf *io = malloc(sizeof(io_buf));
  if (!io) {
    puts("!!Could not allocate IO_BUFFER");
    return NULL;
  }

  io->hr = malloc(sizeof(data_header_record));
  io->br = malloc(sizeof(index_header_record));
  if (!io->hr || !io->br) {
    puts("!!Could not allocate header records");
    free(io);
    return NULL;
  }

  io->hr->free_rrn_address = malloc(sizeof(char) * MAX_ADDRESS);
  io->br->free_rrn_address = malloc(sizeof(char) * MAX_ADDRESS);
  if (!io->hr->free_rrn_address || !io->br->free_rrn_address) {
    puts("!!Could not allocate free_rrn_address");
    free(io->hr);
    free(io->br);
    free(io);
    return NULL;
  }

  if (DEBUG) {
    puts("@Allocated IO_BUFFER");
  }
  return io;
}
void read_data_header(io_buf *io) {
  if (!io || !io->fp) {
    puts("!!Invalid IO buffer or file pointer");
    return;
  }

  data_header_record temp_hr;
  fseek(io->fp, 0, SEEK_SET);
  size_t t = fread(&temp_hr, sizeof(temp_hr.size) + sizeof(temp_hr.record_size), 1, io->fp);
  if (t != 1) {
    puts("!!Error while reading header record (fixed part)");
    return;
  }

  if (!io->hr) {
    io->hr = malloc(sizeof(data_header_record));
    if (!io->hr) {
      puts("!!Memory allocation error");
      return;
    }
  }

  io->hr->record_size = temp_hr.record_size;
  io->hr->size = temp_hr.size;

  size_t rrn_len = io->hr->size - (2 * sizeof(u16));
  io->hr->free_rrn_address = malloc(rrn_len);
  if (!io->hr->free_rrn_address) {
    puts("!!Memory allocation error for free_rrn_address");
    return;
  }

  t = fread(io->hr->free_rrn_address, rrn_len, 1, io->fp);
  if (t != 1) {
    puts("!!Error while reading free_rrn_address");
    free(io->hr->free_rrn_address);
    return;
  }

  if (DEBUG) {
    printf("--> data_header: record_size: %hu size: %hu free_rrn_address: %s\n",
           io->hr->record_size, io->hr->size, io->hr->free_rrn_address);
  }
}

void print_data_record(data_record *hr) {
  printf("Registro de RNN 0\n");
  printf("Placa: %s\n", hr->placa);
  printf("Modelo: %s\n", hr->modelo);
  printf("Marca: %s\n", hr->marca);
  printf("Ano: %d\n", hr->ano);
  printf("Categoria: %s\n", hr->categoria);
  printf("Quilometragem: %d\n", hr->quilometragem);
  printf("Status: %s\n", hr->status);
  printf("---------------------------\n");
}
data_record *read_data_record(io_buf *io, u16 rrn) {
  if (!io || !io->fp) {
    puts("!!Invalid IO buffer or file pointer");
    return NULL;
  }

  data_record *hr = malloc(sizeof(data_record));
  if (!hr) {
    puts("!!Memory allocation failed for data record");
    return NULL;
  }

  int byte_offset = io->hr->size + (io->hr->record_size * rrn);
  fseek(io->fp, byte_offset, SEEK_SET);
  size_t t = fread(hr, sizeof(data_record), 1, io->fp);
  if (t != 1) {
    puts("!!Error while reading data record");
    free(hr);
    return NULL;
  }

  return hr;
}

void write_data_header(io_buf *io) {
  if (!io || !io->fp || !io->hr || !io->hr->free_rrn_address) {
    puts("!!Invalid input in write_data_header");
    return;
  }

  size_t free_rrn_len = strlen(io->hr->free_rrn_address) + 1;
  size_t total_size = sizeof(u16) * 2 + free_rrn_len;

  if (io->hr->size != total_size) {
    io->hr->size = total_size;
  }

  fseek(io->fp, 0, SEEK_SET);
  if (fwrite(&io->hr->size, sizeof(io->hr->size), 1, io->fp) != 1 ||
      fwrite(&io->hr->record_size, sizeof(io->hr->record_size), 1, io->fp) != 1 ||
      fwrite(io->hr->free_rrn_address, free_rrn_len, 1, io->fp) != 1) {
    puts("!!Error while writing header record");
  }

  if (DEBUG) {
    printf("@Successfully written: %hu %hu %s\n", io->hr->record_size, io->hr->size, io->hr->free_rrn_address);
  }
}

void write_data_record(io_buf *io, data_record *d, u16 rrn) {
  if (!io || !io->fp || !d) {
    puts("!!Invalid input in write_data_record");
    return;
  }

  int byte_offset = io->hr->size + (io->hr->record_size * rrn);
  fseek(io->fp, byte_offset, SEEK_SET);
  size_t t = fwrite(d, sizeof(data_record), 1, io->fp);
  if (t != 1) {
    puts("!!Error while writing data record");
  }
}


void populate_header(data_header_record *hp, const char *file_name) {
  if (hp == NULL) {
    puts("!!Header pointer is NULL, cannot populate");
    return;
  }

  hp->record_size = RECORD_SIZE;  
  strncpy(hp->free_rrn_address, file_name, MAX_ADDRESS - 1); 
  hp->free_rrn_address[MAX_ADDRESS - 1] = '\0'; 
  hp->size = strlen(file_name) + 1 + sizeof(u16) * 2;  
}



void load_file(io_buf *io, char *file_name, const char *type) {
  if (!file_name) {
    puts("!!ERROR: no file");
    return;
  }

  if (io->fp != NULL) {
    puts("--> buffer already filled\n--> closing logical link\n");
    if (fclose(io->fp) != 0) {
      puts("!!ERROR: failed to close file");
      return;
    }
    io->fp = NULL; 
  }

  strncpy(io->address, file_name, MAX_ADDRESS - 1);  
  io->address[MAX_ADDRESS - 1] = '\0';  

  printf("@Loading file: %s\n", file_name);

  io->fp = fopen(io->address, "r+b");
  if (io->fp == NULL) {
    puts("!!Error opening file");
    return;
  }

  if (strcmp(type, "index") == 0) {
    if (io->br == NULL) { 
      io->br = malloc(sizeof(index_header_record));
      if (io->br == NULL) {
        puts("!!Memory allocation failed for index_header_record");
        return;
      }
    }
    read_index_header(io);
  } 
  else if (strcmp(type, "data") == 0) {
    if (io->hr == NULL) { 
      io->hr = malloc(sizeof(data_header_record));
      if (io->hr == NULL) {
        puts("!!Memory allocation failed for data_header_record");
        fclose(io->fp);  
        return;
      }
    }
    read_data_header(io);
  } 
  else {
    puts("!!Invalid file type");
    fclose(io->fp); 
    return;
  }

  if (strcmp(type, "data") == 0 && (io->hr->record_size == 0 || io->hr->size == 0)) {
    puts("!!Error: one or more inputs in data_header_record are 0");
    fclose(io->fp);
    return;
  }

  if (strcmp(type, "index") == 0 && (io->br->page_size == 0)) {
    puts("!!Error: one or more inputs in index_header_record are 0");
    fclose(io->fp);
    return;
  }

  puts("@File loaded");
}


void create_data_file(io_buf *io, char *file_name) {
  if (!io || !file_name) {
    puts("!!Invalid IO buffer or file name");
    return;
  }

  strcpy(io->address, file_name);

  if (io->fp != NULL) {
    puts("!!File already open");
    return;
  }

  io->fp = fopen(io->address, "r+b");
  if (!io->fp) {
    printf("!!Error opening file: %s. Creating it...\n", io->address);
    io->fp = fopen(io->address, "wb");  
    if (!io->fp) {
      puts("!!Error creating file");
      return;
    }
    fclose(io->fp); 
    io->fp = fopen(io->address, "r+b");  
    if (!io->fp) { 
      puts("!!Failed to reopen file");
      return;
    }
  }

  if (io->hr == NULL) {
    io->hr = malloc(sizeof(data_header_record));
    if (io->hr == NULL) {
      puts("!!Memory allocation failed for data_header_record");
      fclose(io->fp); 
      return;
    }
  }

  char list_name[MAX_ADDRESS];
  strcpy(list_name, file_name);
  char *dot = strrchr(list_name, '.');
  if (dot) {
    strcpy(dot, ".hlp"); 
  }

  populate_header(io->hr, list_name);
  write_data_header(io);  

  if (DEBUG) {
    puts("@Data file created successfully");
  }
  
}

void clear_io_buf(io_buf *io) {
  if (!io) return;

  if (io->fp) {
    fclose(io->fp);
    io->fp = NULL;
  }

  if (io->hr) {
    free(io->hr->free_rrn_address);
    free(io->hr);
    io->hr = NULL;
  }

  if (io->br) {
    free(io->br->free_rrn_address);
    free(io->br);
    io->br = NULL;
  }

  free(io);
  if (DEBUG) {
    puts("@IO_BUFFER cleared");
  }
}
