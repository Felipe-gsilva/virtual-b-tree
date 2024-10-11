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

#define DISPONIVEL 'disponivel'
#define ALUGADO 'alugado'
#define MANUTENCAO 'em manutencao'

typedef struct data_header_record data_header_record;
typedef struct io_buf io_buf;
typedef struct data_register data_register;

struct data_register {
    // Placa do Veículo (string) - Chave Primária
    char placa[TAMANHO_PLACA];
    // Modelo (string)
    char modelo[TAMANHO_MODELO];
    // Marca (string)
    char marca[TAMANHO_MARCA];
    // Ano (inteiro)
    int ano;
    // Categoria (string) (ex: econômico, luxo, SUV)
    char categoria[TAMANHO_CATEGORIA];
    // Quilometragem (inteiro)
    int quilometragem;
    // Status de Disponibilidade (string) (disponível, alugado, em manutenção)
    char status[TAMANHO_STATUS];
};

struct data_header_record {
    u16 id_size;
    u16 record_size;
    u16 name_size;
    u16 free_rrn;
};


struct io_buf {
    char address[MAX_ADDRESS];
    FILE *fp;
    data_header_record *hr;
};

io_buf *alloc_io_buf();

void print_data_register(data_register *hr);

void load_file(io_buf *io, char *file_name);

void create_data_file(io_buf *io, char *file_name);

void read_data_header(io_buf *io);

data_register *read_data_register(io_buf *io, u16 rrn);

void populate_header(data_header_record *hp);

void write_data_header(io_buf *io);

void write_data_record(io_buf *io, int count, ...);

void clear_io_buf(io_buf *io_buf);

#endif 
