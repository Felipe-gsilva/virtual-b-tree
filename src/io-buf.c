#include "io-buf.h"

io_buf *alloc_io_buf(){
    io_buf *io = malloc(sizeof(io_buf));
    io->hr = malloc(sizeof(header_record));
    if(io) {
        if(DEBUG)
            puts("@Allocated IO_BUFFER");
        return io;
    }
    puts("!!Could not allocate IO_BUFFER");
    return NULL;
}

void read_data_header(io_buf *io) {
    header_record hr[1];
    if (!io->fp) {
        puts("!!File not opened");
        return;
    }

    size_t t = fread(hr, sizeof(header_record),1,io->fp);

    if(t != 1) {
        puts("!!Error while reading header record");
        printf("t size: %zu\n", t);
        return;
    }

    io->hr->record_size = hr->record_size;
    io->hr->id_size = hr->id_size;
    io->hr->name_size = hr->name_size;

    if(DEBUG) {
        puts("@Header Record Loaded");
        printf("-->data_header: %hu %hu %hu\n", io->hr->record_size, io->hr->name_size, io->hr->id_size);
    }
}

/* void read_data_register() {
    data_register hr[1];
    if (!io->fp){
        puts("!!Could not read data header");
        return;
    }

    size_t t = fread(hr, sizeof(header_record),1,io->fp);
    if(t != 1) {
        puts("!!Error while reading header record");
        return;
    }

    io->hr->record_size = hr->record_size;
    io->hr->id_size = hr->id_size;
    io->hr->name_size = hr->name_size;

    if(DEBUG) {
        puts("@Header Record Loaded");
        printf("-->data_header: %hu %hu %hu\n", io->hr->record_size, io->hr->name_size, io->hr->id_size);
    }
}
*/

void write_data_header(FILE *fp, header_record *hr) {
    if (!fp) {
        puts("!!NULL file");
        exit(-1);
    }
    
    if(!hr){
        puts("!!NULL header");
        return;
    }

    size_t flag = fwrite(hr, sizeof(header_record), 1, fp);
    if (flag != 1) 
        puts("!!Error while writing to file");
    if (DEBUG)
        puts("@Successfully written");
}

void populate_header(header_record *hp) {
    if (hp == NULL) {
        puts("!!Header pointer is NULL, cannot populate");
        return;
    }
    hp->record_size = (u16)RECORD_SIZE;
    hp->id_size = (u16)sizeof(u16);
    hp->name_size = (u16)MAX_ADDRESS;
}


void write_data(io_buf *io, int count, ...){
    if(!io || !io->fp || !io->hr) {
        printf("!!NULL io_buf\n");
    }
    va_list args;
    va_start(args, count);
    for (int i = count; i >= 0; i = va_arg(args, int))
        printf("%d ", i);
    va_end(args);
}

void load_file(io_buf *io, char *file_name) {
    if(io->fp != NULL) {
        puts("--> buffer already filled\n--> closing logical link\n");
        fclose(io->fp);
    }

    strcpy(io->name, file_name);
    printf("@Loading file: %s\n", file_name);

    if(!file_name) {
        puts("!!ERROR: no file");
        return;
    }

    io->fp = fopen(io->name, "rb");

    if (io->fp == NULL) {
        printf("Error opening file %s ERROR: %d",io->name, -10);
        exit(1);
    }

    read_data_header(io);
    puts("@File loaded");
}

FILE *create_data_file(char *address) {
    FILE *fp;
    header_record *hp;

    hp = malloc(sizeof(header_record));
    if (hp == NULL) {
        puts("!!Memory allocation failed for header_record");
        return NULL;  
    }

    fp = fopen(address, "wb+");
    if (fp == NULL) {
        printf("!!Error opening file: %s", address);
        free(hp); 
        return NULL; 
    }

    populate_header(hp);
    write_data_header(fp, hp);

    free(hp);  
    return fp;
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

        free(io);
        io = NULL;
    }
    puts("@Buffer cleared");
}
