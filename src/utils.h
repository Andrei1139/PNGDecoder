#pragma once

#include <stdio.h>
#include <stdlib.h>

#define LEN 100

typedef unsigned char byte;

typedef enum {
    GRAYSCALE = 0, RGB = 2, PALETTE = 3, GRAYALPHA = 4, RGBALPHA = 6 
} ColorType;