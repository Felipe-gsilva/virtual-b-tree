#include "io-buf.h"
#include "b-tree-buf.h"

io_buf *alloc_io_buf(){
  io_buf *io = malloc(sizeof(io_buf));
  io->hr = malloc(sizeof(data_header_record));
  io->b = malloc(sizeof(index_header_record));
  if(io) {
    if(DEBUG)
      puts("@Allocated IO_BUFFER");
    return io;
  }
  puts("!!Could not allocate IO_BUFFER");
  return NULL;
}

void read_data_header(io_buf *io) {
  if (!io->fp) {
    puts("!!File not opened");
    return;
  }

  data_header_record *hr;
  fseek(io->fp, 0, SEEK_SET);
  hr = malloc(sizeof(data_header_record));
  int t;
  t = fread(hr, sizeof(data_header_record), 1, io->fp);

  if (t != 1) {
    puts("!!Error while reading header record");
    printf("t size: %d\n", t);
    printf("Read sizes: %hu %hu %hu\n", hr->record_size, hr->id_size, hr->name_size);
    return;
  }

  io->hr->record_size = hr->record_size;
  io->hr->id_size = hr->id_size;
  io->hr->name_size = hr->name_size;

  if(DEBUG) {
    printf("--> data_header: record_size: %hu name_size: %hu id_size: %hu free_rrn: %hu\n", io->hr->record_size, io->hr->name_size, io->hr->id_size, io->hr->free_rrn);
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
    data_record *hr;
    hr = malloc(sizeof(data_record));
    if (!io->fp){
      puts("!!Error: wrong file pointer");
      exit(-1);
    }

    int byte_offset = sizeof(data_header_record) + (io->hr->record_size * rrn);
    fseek(io->fp, byte_offset, SEEK_SET);
    size_t t = fread(hr, sizeof(data_record),1 ,io->fp);

    if(t != 1) {
      puts("!!Error while reading header record");
      return NULL;
    }

  return hr;
}

int retrieve_free_rrn(data_header_record *hr) {
  return 0;
}

void write_data_record(io_buf *io, data_record *d){
  if(!io) {
    puts("!!NULL io_buffer");
    exit(-1);
  }
  if (!io->fp) {
    puts("!!NULL file");
    exit(-1);
  }
  if(!d) {
    puts("!!NULL data record");
    exit(-1);
  }

  int rrn = retrieve_free_rrn(io->hr);
  int byte_offset = sizeof(data_header_record) + (io->hr->record_size * rrn);
  fseek(io->fp, byte_offset, SEEK_SET);
  size_t t = fwrite(d, sizeof(data_record),1 ,io->fp);

  if(t != 1) {
    puts("!!Error while writing header record");
    return;
  }
}

void write_data_header(io_buf *io) {
  if (!io->fp) {
    puts("!!NULL file");
    exit(-1);
  }

  if(io->hr->record_size == 0 || io->hr->name_size == 0 ||io->hr->id_size == 0) {
    puts("!!Error: one or more input on data_header_record are 0");
    return;
  }

  if (io->hr == NULL) {
    io->hr = malloc(sizeof(data_header_record));
    if (io->hr == NULL) {
      puts("!!Memory allocation error");
      return;
    }
  }

  fseek(io->fp, 0, SEEK_SET);
  int flag;
  flag = fwrite(io->hr, sizeof(data_header_record), 1 ,io->fp);

  if (flag != 1){
    puts("!!Error while writing to file");
    if(DEBUG) {
      printf("Flag: %d\n", flag);
      printf("Header: %hu %hu %hu %hu\n", io->hr->record_size, io->hr->id_size, io->hr->name_size, io->hr->free_rrn);
    }
    return;
  }

  if (DEBUG)
    puts("@Successfully written");
}

void populate_header(data_header_record *hp) {
  if (hp == NULL) {
    puts("!!Header pointer is NULL, cannot populate");
    return;
  }

  hp->record_size = RECORD_SIZE;
  hp->id_size = TAMANHO_PLACA;
  hp->name_size = MAX_ADDRESS;
  hp->free_rrn = 999;
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
  }

  strncpy(io->address, file_name, sizeof(io->address) - 1);
  io->address[sizeof(io->address) - 1] = '\0'; 
  printf("@Loading file: %s\n", file_name);

  io->fp = fopen(io->address, "rb");
  if (io->fp == NULL) {
    perror("Error opening file");
    return;
  }

  if(strcmp(type, "index") == 0)
    read_index_header(io);
  if(strcmp(type, "data") == 0)
    read_data_header(io);
  else 
    return;

  if(strcmp(type, "data") == 0 && (io->hr->record_size == 0 ||io->hr->name_size == 0 ||io->hr->id_size == 0)) {
    puts("!!Error: one or more input on data_header_record are 0");
    return;
  }

  if(strcmp(type, "index") == 0 && (io->b->page_size == 0)) {
    puts("!!Error: one or more input on index_header_record are 0");
    return;
  }

  puts("@File loaded");
}


void create_data_file(io_buf *io, char *file_name) {
  strcpy(io->address, file_name);
  if (io->hr == NULL) {
    puts("!!Memory allocation failed for data_header_record");
    return;  
  }

  if(io->fp != NULL || !io)
    exit(-1);

  io->fp = fopen(io->address, "r+b");
  if (!io->fp) {
    printf("!!Error opening file: %s", io->address);
    return; 
  }

  populate_header(io->hr);
  if(io->hr != NULL) {
    write_data_header(io);
    return;
  }

  puts("!!Error could craete data file");
}

void clear_io_buf(io_buf *io) {
  if (io) {
    if (io->fp) {
      fclose(io->fp);
      io->fp = NULL;  
    }

    if (io->hr) {
      free(io->hr);
      io->hr = NULL; 
    }
    if(io->b) {
      free(io->b);
      io->b = NULL; 
    }

    free(io);
    io = NULL;
  }
  if(DEBUG)
    puts("@IO_BUFFERcleared");
}
