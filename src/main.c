#include "image.h"

int main() {
    system("cls");

    char filename[LEN], path[2 * LEN];
    printf("Insert image file name: ");
    scanf("%100s", filename);

    if (strcmp(filename + strlen(filename) - 4, ".png") != 0) {
        fprintf(stderr, "Only PNG files allowed\n");
        return -1;
    }

    sprintf(path, "../../../images/%s", filename);

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        perror(NULL);
        return -1;
    }

    // Skip the 8 byte header, thus jumping straight to the file's chunks
    long int offset = 8;
    if (fseek(file, offset, SEEK_SET) != 0) {
        perror(NULL);
        fclose(file);
        return -1;
    }

    // Create vector with all chunks
    Chunk *chunk_vec = malloc(ALLOC_PCK * sizeof(Chunk));
    size_t vec_size = 0, vec_capacity = ALLOC_PCK;
    while (1) {
        chunk_vec[vec_size] = init_chunk(&file);

        if (IS_END(chunk_vec[vec_size].type)) {
            ++vec_size;
            break;
        }

        if (++vec_size == vec_capacity) {
            vec_capacity += ALLOC_PCK;
            void *aux_ptr = realloc(chunk_vec, vec_capacity * sizeof(Chunk *));
            if (aux_ptr == NULL) {
                perror(NULL);
                free_chunks(chunk_vec, vec_size + 1);
                fclose(file);
                return -1;
            }
            chunk_vec = aux_ptr;
        }
    }

    process_chunks(chunk_vec, vec_size);

    fclose(file);
    free_chunks(chunk_vec, vec_size);
}