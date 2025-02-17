#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define LEN 100
#define ALLOC_PCK 1000

#define IS_END(x) (x[0] == 73 && x[1] == 69 && x[2] == 78 && x[3] == 68)
#define SWITCH_INT_ENDIANS(x) ((x << 24) | (x << 8 & 255 << 16) | (x >> 8 & 255 << 8) | (x >> 24))
#define BITS_PER_BYTE 8

typedef unsigned char byte;

typedef struct __attribute__((packed)){
    unsigned int length;
    byte type[4];
    byte *data;
    byte crc[4];
} Chunk;

Chunk init_chunk(FILE **file);
void free_chunks(Chunk *vec, size_t size);

void print_chunk(Chunk *chunk);
void process_chunks(Chunk *chunks, size_t size);