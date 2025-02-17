#include "image.h"

void print_chunk(Chunk *chunk) {
    printf("Length: %u\nType: %.4s", chunk->length, chunk->type);
    // for (int i = 0; i < chunk->length; ++i) {
    //     printf("%02d ", chunk->data[i]);
    // }

    printf("\nCRC: ");
    
    for (int i = 0; i < 4; ++i) {
        printf("%02d ", chunk->crc[i]);
    }
}

Chunk init_chunk(FILE **file) {
    Chunk chunk;
    
    chunk.length = 0;
    for (int i = 3; i >= 0; --i) {
        chunk.length += fgetc(*file) << BITS_PER_BYTE * i;
    }

    for (int i = 0; i < 4; ++i) {
        chunk.type[i] = (byte)fgetc(*file);
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

void process_chunks(Chunk *chunks, size_t size) {
    Chunk header_chunk = chunks[0];

    unsigned width = SWITCH_INT_ENDIANS(*(unsigned *)(header_chunk.data));
    unsigned height = SWITCH_INT_ENDIANS(*(unsigned *)(header_chunk.data + 4));

    
}

void free_chunks(Chunk *vec, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        free(vec[i].data);
    }
    free(vec);
}