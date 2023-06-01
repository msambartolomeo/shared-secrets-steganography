#include "bmp.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define OFFSET_INDEX 0x0A
#define IMAGE_SIZE_INDEX 0x22

uint32_t bytes_to_u32(uint8_t *buff);

BmpImage *parse_bmp(const char *path, size_t k) {
    BmpImage *bmp = malloc(sizeof(BmpImage));
    if (bmp == NULL) {
        return NULL;
    }

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        free(bmp);
        return NULL;
    }

    // NOTE: Read header
    size_t bytes_read = fread(bmp->header, 1, HEADER_SIZE, file);
    if (bytes_read != HEADER_SIZE || bmp->header[0] != 'B' ||
        bmp->header[1] != 'M') {
        free(bmp);
        fclose(file);
        errno = EINVAL;
        return NULL;
    }

    uint32_t offset = bytes_to_u32(&bmp->header[OFFSET_INDEX]);

    // NOTE: Read extra info
    uint32_t data_size = offset - HEADER_SIZE;
    bmp->extra = malloc(sizeof(uint8_t) * data_size);
    if (bmp->extra == NULL) {
        free(bmp);
        fclose(file);
        return NULL;
    }
    bytes_read = fread(bmp->extra, 1, data_size, file);
    if (bytes_read != data_size) {
        free(bmp->extra);
        free(bmp);
        fclose(file);
        errno = EINVAL;
        return NULL;
    }

    // NOTE: Read image
    uint32_t image_size = bytes_to_u32(&bmp->header[IMAGE_SIZE_INDEX]);
    uint8_t *image = malloc(sizeof(uint8_t) * image_size);
    if (image == NULL) {
        free(bmp->extra);
        free(bmp);
        fclose(file);
        return NULL;
    }
    bytes_read = fread(image, 1, image_size, file);
    if (bytes_read != image_size) {
        free(image);
        free(bmp->extra);
        free(bmp);
        fclose(file);
        errno = EINVAL;
        return NULL;
    }

    // NOTE: Separate image in blocks
    uint32_t block_size = 2 * k - 2;
    if (image_size % block_size != 0) {
        free(image);
        free(bmp->extra);
        free(bmp);
        fclose(file);
        errno = EINVAL;
        return NULL;
    }
    bmp->block_count = image_size / block_size;
    bmp->blocks = malloc(sizeof(BmpBlock) * bmp->block_count);
    if (bmp->blocks == NULL) {
        free(image);
        free(bmp->extra);
        free(bmp);
        fclose(file);
        errno = EINVAL;
        return NULL;
    }
    for (size_t i = 0; i < bmp->block_count; i++) {
        bmp->blocks[i] = &image[i * block_size];
    }

    fclose(file);
    return bmp;
}

void free_bmp(BmpImage *bmp) {
    free(bmp->blocks[0]);
    free(bmp->blocks);
    free(bmp->extra);
    free(bmp);
}

uint32_t bytes_to_u32(uint8_t *buff) {
    uint32_t value = 0;
    for (int i = 0; i < 4; i++) {
        value |= (buff[i] << (8 * i));
    }
    return value;
}
