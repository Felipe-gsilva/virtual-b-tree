#ifndef FILE_H
#define FILE_H

#include "defines.h"
//
// in bytes
#define MAX_ADDRESS 4096 
#define RECORD_SIZE 88 

// size data register 
#define TAMANHO_PLACA 8
#define TAMANHO_MODELO 20
#define TAMANHO_MARCA 20
#define TAMANHO_CATEGORIA 15
#define TAMANHO_STATUS 16

typedef struct header_record header_record;
typedef struct io_buf io_buf;
typedef struct data_register data_register;

struct data_register {
    char placa[TAMANHO_PLACA];
    char modelo[TAMANHO_MODELO];
    char marca[TAMANHO_MARCA];
    int ano;
    char categoria[TAMANHO_CATEGORIA];
    int quilometragem;
    char status[TAMANHO_STATUS];
// Placa do Veículo (string) - Chave Primária
// Modelo (string)
// Marca (string)
// Ano (inteiro)
// Categoria (string) (ex: econômico, luxo, SUV)
// Quilometragem (inteiro)
// Status de Disponibilidade (string) (disponível, alugado, em manutenção)

};
struct header_record {
    u16 record_size;
    u16 id_size;
    u16 name_size;
};

struct io_buf {
    char name[MAX_ADDRESS];
    FILE *fp;
    header_record *hr;
};

io_buf *alloc_io_buf();

void load_file(io_buf *io, char *file_name);

FILE *create_data_file(char *address);

void read_data_header(io_buf *io);

void populate_header(header_record *hp);

void write_data_header(FILE *fp, header_record *hp);

void write_data(io_buf *io, int count, ...);

void clear_io_buf(io_buf *io_buf);

#endif 
