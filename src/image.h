#pragma ocne

#include "utils.h"
#include "print.h"

int generate_image(RGBValue **image, size_t width, size_t height, byte *data,
    size_t size, ColorType color, byte bit_depth, Chunk *pallet);