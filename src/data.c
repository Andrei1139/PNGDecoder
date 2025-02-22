#include "data.h"

void print_chunk(Chunk *chunk) {
    printf("Length: %u\nType: %.4s", chunk->length, chunk->type);
    // for (int i = 0; i < chunk->length; ++i) {
    //     printf("%02d ", chunk->data[i]);
    // }

    printf("\nCRC: ");
    
    for (int i = 0; i < 4; ++i) {
        printf("%02d ", chunk->crc[i]);
    }

    printf("\n\n");
}

Chunk init_crit_chunk(FILE **file) {
    Chunk chunk;

    while (1) {
        chunk.length = 0;
        for (int i = 3; i >= 0; --i) {
            chunk.length += fgetc(*file) << BITS_PER_BYTE * i;
        }

        for (int i = 0; i < 4; ++i) {
            chunk.type[i] = (byte)fgetc(*file);
        }

        if ((chunk.type[0] & (1 << 5)) == 0) {
            break;
        }

        // Restart initialization if chunk is not critical
        // For the next chunk:
        fseek(*file, chunk.length + 4, SEEK_CUR);
    }

    chunk.data = malloc(chunk.length * sizeof(byte));
    for (int i = 0; i < chunk.length; ++i) {
        chunk.data[i] = (byte)fgetc(*file);
    }

    for (int i = 0; i < 4; ++i) {
        chunk.crc[i] = (byte)fgetc(*file);
    }

    return chunk;
}

byte *decompress_image_data(byte *data, size_t *size) {
    FILE *in = fopen("temp.in", "wb");
    for (size_t i = 0; i < *size; ++i) {
        fputc(data[i], in);
    }
    fclose(in);

    in = fopen("temp.in", "rb");
    FILE *out = fopen("temp.out", "wb");

    unsigned int aux;

    inflate(in, out, &aux);

    fclose(in);
    fclose(out);

    out = fopen("temp.out", "rb");
    byte input;

    fseek(out, 0, SEEK_END);
    *size = ftell(out);
    fseek(out, 0, SEEK_SET);

    byte *new_data = malloc(*size * sizeof(byte));

    for (size_t i = 0; i < *size; ++i) {
        new_data[i] = (byte)fgetc(out);
    }

    return new_data;
}

size_t get_bpp(byte color_arg, byte bit_depth_arg) {
    size_t bit_depth = bit_depth_arg;
    ColorType color_type = color_arg;

    size_t samples_per_pixel;

    switch (color_type) {
        case GRAYSCALE: case PALETTE:
            samples_per_pixel = 1;
            break;
        case GRAYALPHA:
            samples_per_pixel = 2;
            break; 
        case RGB:
            samples_per_pixel = 3;
            break;
        case RGBALPHA:
            samples_per_pixel = 4;
            break;
        default:
            fprintf(stderr, "Invalid color\n");
            return 0;
    }

    return ceil((double)samples_per_pixel * bit_depth / BITS_PER_BYTE);
}

int process_chunks(Chunk *chunks, size_t size) {
    Chunk *header_chunk = &chunks[0];

    uint32_t width = SWITCH_INT_ENDIANS(*(uint32_t *)(header_chunk->data));
    uint32_t height = SWITCH_INT_ENDIANS(*(uint32_t *)(header_chunk->data + 4));
    // Num or bits per sample/palette index (1, 2, 4, 8 or 16)
    byte bit_depth = (header_chunk->data + 8)[0];
    // 1 - palette used, 2 - color used, 4 - alpha channel used +->
    // 0, 2, 3, 4 or 6
    byte color_type = (header_chunk->data + 9)[0];
    byte compression_method = (header_chunk->data + 10)[0];
    byte filter_method = (header_chunk->data + 11)[0];
    byte interlace_method = (header_chunk->data + 12)[0];

    // Only specific combinations allowed (last condition applies to 2, 4 and 6
    // (either using RGB or alpha sample))
    if (bit_depth > 0b1000 && color_type == PALETTE || bit_depth < 0b1000 &&
        color_type % 2 == 0 && color_type != GRAYSCALE) {
        fprintf(stderr, "Invalid color type + bit depth combination\n");
        return -1;
    }

    if (compression_method != 0) { // Only one compression method (0) allowed
        fprintf(stderr, "Unrecognized compression code\n");
        return -1;
    }

    if (filter_method != 0) { // Only one filter method (0) allowed
        fprintf(stderr, "Unrecognized filter code\n");
        return -1;
    }

    if (interlace_method != 0) { // Adam7 interlacing not currently supported
        fprintf(stderr, "File uses interlacing method not currently supported\n");
        return -1;
    }

    Chunk *pallet = IS_PALLET(chunks[1].type) ? &chunks[1] : NULL;

    if (pallet != NULL) {
        // Since data contains three-byte series (RGB values), length must be multiple of 3
        if (pallet->length % 3 != 0) {
            fprintf(stderr, "Invalid pallet\n");
            return -1;
        }

        RGBValue pallet_colors[pallet->length / 3];
        memcpy(pallet_colors, pallet->data, pallet->length);

        // for (int i = 0; i < pallet->length / 3; ++i) {
        //     printf("Red: %02d, Green: %02d, Blue: %02d\n", pallet_colors[i].red, pallet_colors[i].green, pallet_colors[i].blue);
        // }
    }

    size_t data_size = 0;
    for (size_t i = (pallet == NULL ? 1 : 2); i < size; ++i) {
        data_size += chunks[i].length;
    }

    // Get entire image data by concatenating all IDAT chunks' datas
    byte data[data_size];
    size_t position = 0;
    for (size_t i = (pallet == NULL ? 1 : 2); i < size; ++i) {
        memcpy(data + position, chunks[i].data, chunks[i].length);
        position += chunks[i].length;
    }

    // Decompressing...
    byte *decompressed_data = decompress_image_data(data, &data_size);

    printf("Size of decompressed data: %ld\n", data_size);

    // Filtering
    // First calculating the bpp (bytes per complete pixel)...
    size_t bpp = get_bpp(color_type, bit_depth);

    // ...then defiltering the data...
    byte *raw_data = defilter(decompressed_data, &data_size, height, bpp);

    // Converting data to matrix of RGB values
    RGBValue **image = malloc(height * sizeof(RGBValue *));
    for (size_t i = 0; i < height; ++i) {
        image[i] = malloc(width * sizeof(RGBValue));
    }

    generate_image(image, width, height, raw_data, data_size, color_type, bit_depth, pallet);

    // Cleaning up...
    free(raw_data);
    free(decompressed_data);
    for (size_t i = 0; i < height; ++i) {
        free(image[i]);
    }
    free(image);

    return 0;
}

void free_chunks(Chunk *vec, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        free(vec[i].data);
    }
    free(vec);
}