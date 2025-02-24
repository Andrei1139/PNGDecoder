#include "image.h"

static byte bit_index = 7;

static void advance(byte **data, byte bits) {
    if (bits < 8) {
        **data << bits;
        bit_index -= bits;
    } else {
        *data += bits / 8;
    }

    if (bit_index > 7) { // If the data vector must be modified (applies only for bits < 8)
        ++*data;
        bit_index = bit_index % 8;
    }
}

static byte get_pixel_sample(byte *data, byte bits) {
    if (bits == 8) {
        return *data;
    }

    byte mask = 1 << bits - 1; // 1 for 1 bit, 11 for 2 bits, 1111 for 4 bits
    mask = mask << 8 - bits; // 1000 000 for 1 bit, 1100 0000 for 2 bits etc.

    byte sample = *data & mask;
    sample = sample >> 8 - bits;

    return sample;
}

static void get_grayscale_rgb(RGBValue *value, byte **data, byte bit_depth, int has_alpha) {
    byte sample = get_pixel_sample(*data, bit_depth);
    advance(data, bit_depth);
    if (has_alpha) { // Alpha ignored, but taken into consideration when modifying data
        advance(data, bit_depth);
    }

    byte max_sample = 1 << bit_depth - 1; // Max value for the sample

    byte individual_value = sample * 255 / max_sample; // Value for each color

    value->red = value->green = value->blue = individual_value;
}

static void get_palette_rgb(RGBValue *value, byte **data, byte bit_depth, Chunk *pallet) {
    byte index = get_pixel_sample(*data, bit_depth);
    
    byte *pos = &(pallet->data[index * 3]);
    value->red = pos[0];
    value->green = pos[1];
    value->blue = pos[2];

    advance(data, bit_depth);
}

static void get_rgb(RGBValue *value, byte **data, int has_alpha) {
    value->red = (*data)[0];
    value->green = (*data)[1];
    value->blue = (*data)[2];

    advance(data, 8 * 3);
    if (has_alpha) {
        advance(data, 8);
    }
}

int generate_image(RGBValue **image, size_t width, size_t height, byte *data,
size_t size, ColorType color, byte bit_depth, Chunk *pallet) {
    byte *cpy = data;
    if (pallet == NULL && color == PALETTE) {
        fprintf(stderr, "Pallete not found, but required by file\n");
        return -1;
    }

    if (bit_depth > 8) {
        fprintf(stderr, "Bit depths larger than 8 not supported\n");
        return -1;
    }

    if (color != RGB && color != PALETTE) {
        fprintf(stderr, "Only simple RGB and palette coloring allowed\n");
        return -1;
    }

    for (size_t row = 0; row < height; ++row) {
        for (size_t column = 0; column < width; ++column) {
            switch (color) {
                case PALETTE:
                    get_palette_rgb(&image[row][column], &data, bit_depth, pallet);
                    break;
                case RGB:
                    get_rgb(&image[row][column], &data, 0);
                    break;
                case RGBALPHA:
                    get_rgb(&image[row][column], &data, 1);
                    break;
                case GRAYSCALE:
                    get_grayscale_rgb(&image[row][column], &data, bit_depth, 0);
                    break;
                case GRAYALPHA:
                    get_grayscale_rgb(&image[row][column], &data, bit_depth, 1);
                    break;
            }
        }
    }

    print_RGB(image, width, height, "output.txt");

    return 0;
}