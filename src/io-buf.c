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
    if (!io->fp) {
        puts("!!File not opened");
        return;
    }

    header_record *hr;
    fseek(io->fp, 0, SEEK_SET);

    hr = malloc(sizeof(header_record));
    int t = fread(hr, sizeof(header_record),1 ,io->fp);

    if(t != 1) {
        puts("!!Error while reading header record");
        printf("t size: %d\n", t);
        printf("Read sizes: %hu %hu %hu\n", hr->record_size, hr->id_size, hr->name_size);
        return;
    }
    
    io->hr->record_size = hr->record_size;
    io->hr->id_size = hr->id_size;
    io->hr->name_size = hr->name_size;

    if(DEBUG) {
        puts("@Header Record Loaded");
        printf("-->data_header: %hu %hu %hu\n", io->hr->record_size, io->hr->name_size, io->hr->id_size);
        printf("Read sizes: %hu %hu %hu\n", hr->record_size, hr->id_size, hr->name_size);
    }
    free(hr);
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

void write_data_header(io_buf *io) {
    if (!io->fp) {
        puts("!!NULL file");
        exit(-1);
    }
    
    if(!io->hr){
        puts("!!NULL header");
        return;
    }

    fseek(io->fp, 0, SEEK_SET);
    size_t flag = fwrite(io->hr, sizeof(header_record), 1, io->fp);

    if (flag != 1) {
        puts("!!Error while writing to file");
        return;   
    }
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

    strcpy(io->address, file_name);
    printf("@Loading file: %s\n", file_name);

    if(!file_name) {
        puts("!!ERROR: no file");
        return;
    }

    io->fp = fopen(io->address, "rb");

    if (io->fp == NULL) {
        printf("Error opening file %s ERROR: %d",io->address, -10);
        exit(1);
    }

    read_data_header(io);
    puts("@File loaded");
}

void create_data_file(io_buf *io, char *file_name) {
    strcpy(io->address, file_name);
    if (io->hr == NULL) {
        puts("!!Memory allocation failed for header_record");
        return;  
    }

    if(io->fp != NULL)
        exit(-1);

    io->fp = fopen(io->address, "wb+");
    if (!io->fp) {
        printf("!!Error opening file: %s", io->address);
        return; 
    }

    populate_header(io->hr);
    write_data_header(io);
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
