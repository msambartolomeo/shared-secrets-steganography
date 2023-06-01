#include "bmp.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define OFFSET_INDEX 0x0A
#define IMAGE_SIZE_INDEX 0x22

uint32_t bytes_to_u32(uint8_t *buff);

BmpImage *parse_bmp(const char *path) {
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
    if (bytes_read != HEADER_SIZE) {
        free(bmp);
        fclose(file);
        errno = EINVAL;
        return NULL;
    }

    uint32_t offset = bytes_to_u32(&bmp->header[OFFSET_INDEX]);

    // NOTE: Read extra info
    uint32_t data_size = offset - HEADER_SIZE;
    bmp->extra = malloc(sizeof(uint8_t) * data_size);
    bytes_read = fread(bmp->extra, 1, data_size, file);
    if (bytes_read != data_size) {
        free(bmp);
        fclose(file);
        errno = EINVAL;
        return NULL;
    }

    // NOTE: Read image
    uint32_t image_size = bytes_to_u32(&bmp->header[IMAGE_SIZE_INDEX]);
    bmp->image = malloc(sizeof(uint8_t) * image_size);
    bytes_read = fread(bmp->image, 1, data_size, file);
    if (bytes_read != data_size) {
        free(bmp);
        fclose(file);
        errno = EINVAL;
        return NULL;
    }

    fclose(file);
    return bmp;
}

void free_bmp(BmpImage *bmp) {
    free(bmp->image);
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
