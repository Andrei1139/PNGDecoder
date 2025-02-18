#pragma once

typedef enum {
    NONE = 0, SUB = 1, UP = 2, AVERAGE = 3, PAETH = 4
} FilterType;

int defilter(byte *data, size_t width, size_t height);