#include "bmp.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define OFFSET_INDEX 0x0A
#define IMAGE_SIZE_INDEX 0x22
#define RESERVED_INDEX 0x06

uint32_t bytes_to_u32(uint8_t *buff);

BmpImage *parse_bmp(const char *path, uint8_t k) {
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
    bmp->extra_size = offset - HEADER_SIZE;
    bmp->extra = malloc(sizeof(uint8_t) * bmp->extra_size);
    if (bmp->extra == NULL) {
        free(bmp);
        fclose(file);
        return NULL;
    }
    bytes_read = fread(bmp->extra, 1, bmp->extra_size, file);
    if (bytes_read != bmp->extra_size) {
        free(bmp->extra);
        free(bmp);
        fclose(file);
        errno = EINVAL;
        return NULL;
    }

    // NOTE: Verify image size
    bmp->image_size = bytes_to_u32(&bmp->header[IMAGE_SIZE_INDEX]);
    if (bmp->image_size % (2 * k - 2) != 0) {
        fprintf(stderr, "Image size of file %s is not divisible by (2k-2).\n",
                path);
        free(bmp->extra);
        free(bmp);
        fclose(file);
        errno = EINVAL;
        return NULL;
    }

    // NOTE: Read image
    bmp->image = malloc(sizeof(uint8_t) * bmp->image_size);
    if (bmp->image == NULL) {
        free(bmp->extra);
        free(bmp);
        fclose(file);
        return NULL;
    }
    bytes_read = fread(bmp->image, 1, bmp->image_size, file);
    if (bytes_read != bmp->image_size) {
        free(bmp->image);
        free(bmp->extra);
        free(bmp);
        fclose(file);
        errno = EINVAL;
        return NULL;
    }

    fclose(file);
    return bmp;
}

int output_bmp(BmpImage *bmp, const char *path) {
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        return 1;
    }

    uint32_t bytes_written = fwrite(bmp->header, 1, HEADER_SIZE, file);
    if (bytes_written != HEADER_SIZE) {
        fclose(file);
        return 1;
    }

    bytes_written = fwrite(bmp->extra, 1, bmp->extra_size, file);
    if (bytes_written != bmp->extra_size) {
        fclose(file);
        return 1;
    }

    bytes_written = fwrite(bmp->image, 1, bmp->image_size, file);
    if (bytes_written != bmp->image_size) {
        fclose(file);
        return 1;
    }
    return 0;
}

void free_bmp(BmpImage *bmp) {
    free(bmp->image);
    free(bmp->extra);
    free(bmp);
}

void save_shadow_number(BmpImage *bmp, uint8_t shadow_number) {
    bmp->header[RESERVED_INDEX] = shadow_number;
}

uint8_t get_shadow_number(BmpImage *bmp) { return bmp->header[RESERVED_INDEX]; }

uint32_t bytes_to_u32(uint8_t *buff) {
    uint32_t value = 0;
    for (int i = 0; i < 4; i++) {
        value |= (buff[i] << (8 * i));
    }
    return value;
}
