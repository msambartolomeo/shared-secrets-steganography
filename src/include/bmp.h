#pragma once
#include <stdint.h>
#include <stdlib.h>

#define HEADER_SIZE 54

typedef uint8_t *BmpBlock;

typedef struct BmpImage {
    uint8_t header[HEADER_SIZE];
    uint8_t *extra;

    uint32_t block_count;
    BmpBlock *blocks;
} BmpImage;

/*
    Parses bmp image from path and divides it into blocks of length 2k-2
    Returns the image in a `BmpImage` struct.
    On error `NULL` is returned and `errno` is set.
    if `errno` = `EINVAL` then the image is not a valid bmp image.
*/
BmpImage *parse_bmp(const char *path, size_t k);

/*
    Frees the `BmpImage` struct
*/
void free_bmp(BmpImage *bmp);
