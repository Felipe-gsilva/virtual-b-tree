#ifndef _DEFINES_H
#define _DEFINES_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 1 // 1 for dev mode, 0 for prod mode
#define ORDER 3

#define check puts("Breakdown here");
// in bytes
#define MAX_ADDRESS 4096

// data record_size
#define RECORD_SIZE 88

// size data record
#define TAMANHO_PLACA 8
#define TAMANHO_MODELO 20
#define TAMANHO_MARCA 20
#define TAMANHO_CATEGORIA 15
#define TAMANHO_STATUS 16

// status carro
#define DISPONIVEL "disponivel"
#define ALUGADO "alugado"
#define MANUTENCAO "em manutencao"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef struct index_header_record index_header_record;
typedef struct data_header_record data_header_record;
typedef struct b_tree_buf b_tree_buf;
typedef struct io_buf io_buf;
typedef struct index_record index_record;
typedef struct data_record data_record;
typedef struct queue queue;
typedef struct key key;
typedef struct page page;
typedef struct app app;
typedef struct i_list i_list;

struct key {
  u16 data_register_rrn;
  char id[TAMANHO_PLACA];
};

struct page {
  u16 rrn;
  key keys[ORDER - 1];
  u16 children[ORDER];
  u8 child_number;
  u16 father;
  bool leaf;
};

struct queue {
  queue *next;
  page *page;
  u16 counter;
};

struct data_record {
  char placa[TAMANHO_PLACA];
  char modelo[TAMANHO_MODELO];
  char marca[TAMANHO_MARCA];
  int ano;
  char categoria[TAMANHO_CATEGORIA];
  int quilometragem;
  char status[TAMANHO_STATUS]; // Status de Disponibilidade (disponível,
                               // alugado, em manutenção)
};

struct data_header_record {
  u16 size;
  u16 record_size;
  char *free_rrn_address;
};

struct index_header_record {
  u16 root_rrn;
  u16 page_size;
  u16 size;
  char *free_rrn_address;
};

struct io_buf {
  char address[MAX_ADDRESS];
  FILE *fp;
  data_header_record *hr;
  index_header_record *br;
};

struct b_tree_buf {
  page *root;
  io_buf *io;
  queue *q;
  i_list *i;
};

struct i_list {
  io_buf *io;
  u16 *free_rrn;
  u16 n;
};

struct app {
  io_buf *in;
  io_buf *out;
  b_tree_buf *b;
};

#endif
