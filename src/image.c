#include "image.h"

typedef uint16_t sample;

static void advance(byte **data, byte *bit_index, byte bits) {
    switch (bits) {
        case 1:
            --*bit_index;
            break;
        case 2:
            *bit_index -= 2;
            break;
        case 4:
            *bit_index -= 4;
            break;
        default:
            *data += bits / 8;
            break;
    }

    if (*bit_index == 255) { // If data must be modified (applies only for bits < 8)
        ++*data;
        *bit_index = 0;
    }
}

static void get_palette_rgb(RGBValue *value, byte index, Chunk *pallet) {
    byte *pos = &(pallet->data[index * 3]);
    value->red = pos[0];
    value->green = pos[1];
    value->blue = pos[2];
}

static void get_rgb(RGBValue *value, byte *data) {
    value->red = data[0];
    value->green = data[1];
    value->blue = data[2];
}

int generate_image(RGBValue **image, size_t width, size_t height, byte *data,
size_t size, ColorType color, byte bit_depth, Chunk *pallet) {
    byte *cpy = data;
    if (pallet == NULL && color == PALETTE) {
        fprintf(stderr, "Pallete not found, but required by file\n");
        return -1;
    }

    if (bit_depth != 8) {
        fprintf(stderr, "Bit depths other than 8 not supported yet\n");
        return -1;
    }

    if (color != RGB && color != PALETTE) {
        fprintf(stderr, "Only simple RGB and palette coloring allowed\n");
        return -1;
    }

    // sample value, max_value, rgbs[3];
    byte bit_index = 7;

    for (size_t row = 0; row < height; ++row) {
        for (size_t column = 0; column < width; ++column) {
            switch (color) {
                case PALETTE:
                    get_palette_rgb(&image[row][column], *data, pallet);
                    advance(&data, &bit_index, bit_depth);
                    break;
                case RGB:
                    get_rgb(&image[row][column], data);
                    advance(&data, &bit_index, bit_depth * 3);
                    break;
            }
        }
    }

    print_RGB(image, width, height, "output.txt");

    return 0;
}