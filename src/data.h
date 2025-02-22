#pragma once

#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include "utils.h"
#include "kinflate/kinflate.h"
#include "defilter.h"
#include "image.h"
#include "print.h"
#define ALLOC_PCK 1000

#define IS_END(x) (strncmp(x, "IEND", 4) == 0)
#define IS_PALLET(x) (strncmp(x, "PLTE", 4) == 0)
#define IS_DATA(x) (strncmp(x, "IDAT") == 0)
#define SWITCH_INT_ENDIANS(x) ((x << 24) | (x << 8 & 255 << 16) | (x >> 8 & 255 << 8) | (x >> 24))
#define BITS_PER_BYTE 8

Chunk init_crit_chunk(FILE **file);
void free_chunks(Chunk *vec, size_t size);

void print_chunk(Chunk *chunk);
int process_chunks(Chunk *chunks, size_t size);