#pragma once

#include <stddef.h>
#include "utils.h"

typedef enum {
    NONE = 0, SUB = 1, UP = 2, AVERAGE = 3, PAETH = 4
} FilterType;

byte *defilter(byte *data, size_t *size, size_t height, size_t bpp);