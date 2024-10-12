#ifndef _DEFINES_H
#define _DEFINES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#define DEBUG 0
#define ORDER 4

// in bytes
#define MAX_ADDRESS 4096 
#define RECORD_SIZE 88 

// size data record 
#define TAMANHO_PLACA 8
#define TAMANHO_MODELO 20
#define TAMANHO_MARCA 20
#define TAMANHO_CATEGORIA 15
#define TAMANHO_STATUS 16

// status carro
#define DISPONIVEL 'disponivel'
#define ALUGADO 'alugado'
#define MANUTENCAO 'em manutencao'

typedef uint8_t  u8; 
typedef uint16_t u16;  
typedef uint32_t u32; 
typedef uint64_t u64; 

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef struct b_tree_buf b_tree_buf;
typedef struct index_header_record index_header_record;
typedef struct data_header_record data_header_record;
typedef struct io_buf io_buf;
typedef struct index_record index_record;
typedef struct data_record data_record;
typedef struct queue queue;
typedef struct app app;
typedef struct key key;
typedef struct page page;

struct key {
  u16 data_register_rrn;
  char id[TAMANHO_PLACA];
};

struct page{
  u16 rrn;
  key keys[ORDER-1];            
  u16 children[ORDER];
  u8 child_number;
  u16 father;
};



struct queue {
  queue *next;
  page *page;
  u8 counter;
};

struct data_record {
  char placa[TAMANHO_PLACA]; // key
  // Modelo (string)
  char modelo[TAMANHO_MODELO];
  // Marca (string)
  char marca[TAMANHO_MARCA];
  int ano;
  // Categoria (string) (ex: econômico, luxo, SUV)
  char categoria[TAMANHO_CATEGORIA];
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

struct index_header_record {
  u16 root_rrn;
  u16 page_size;
};

struct io_buf {
  char address[MAX_ADDRESS];
  FILE *fp;
  data_header_record *hr;
  index_header_record *b;
};

struct b_tree_buf {
  index_header_record *bh;
  io_buf *io;
  queue *q;
  page *root;
};

struct app {
  io_buf *in;
  io_buf *out;
  b_tree_buf *b;
};
#endif
