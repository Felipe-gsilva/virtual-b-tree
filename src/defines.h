#ifndef _DEFINES_H
#define _DEFINES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#define DEBUG 1

// in bytes
#define MAX_ADDRESS 4096 
#define RECORD_SIZE 50 

// size data register 
#define TAMANHO_PLACA 8
#define TAMANHO_MODELO 20
#define TAMANHO_MARCA 20
#define TAMANHO_CATEGORIA 15
#define TAMANHO_STATUS 16

// queue max
#define P 50

typedef uint8_t  u8; 
typedef uint16_t u16;  
typedef uint32_t u32; 
typedef uint64_t u64; 

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#endif
