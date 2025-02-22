#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define LEN 100

typedef unsigned char byte;

typedef enum {
    GRAYSCALE = 0, RGB = 2, PALETTE = 3, GRAYALPHA = 4, RGBALPHA = 6 
} ColorType;

typedef struct __attribute__((packed)){
    byte red, green, blue;
} RGBValue;

typedef struct __attribute__((packed)){
    uint32_t length;
    byte type[4];
    byte *data;
    byte crc[4];
} Chunk;