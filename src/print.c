#include "print.h"

static float get_rgb_to_greyscale(RGBValue value) {
    uint16_t sum = value.red + value.green + value.blue;
    return (float)sum / (255 * 3);
}

void print_data(byte *data, size_t width, size_t height, char *filename) {
    FILE *output = fopen(filename, "w");
    if (output == NULL) {
        perror(NULL);
        exit(-1);
    }

    printf("%ld %ld\n", width, height);

    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            fprintf(output, "%03d ", data[i * width + j]);
        }
        fprintf(output, "\n");
    }

    fclose(output);
}

void print_RGB(RGBValue **data, size_t width, size_t height, char *filename) {
    FILE *output = fopen(filename, "w");
    if (output == NULL) {
        perror(NULL);
        exit(-1);
    }

    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            fprintf(output, "%.2f ", get_rgb_to_greyscale(data[i][j]));
        }
        fprintf(output, "\n");
    }

    fclose(output);
}