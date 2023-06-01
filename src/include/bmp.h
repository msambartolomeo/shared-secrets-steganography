#pragma once
#include <stddef.h>
#include <stdint.h>

#define HEADER_SIZE 54

typedef struct BmpImage {
    uint8_t header[HEADER_SIZE];
    uint8_t *extra;
    size_t extra_size;
    uint8_t *image;
    size_t image_size;
} BmpImage;

/*
    Parses bmp image from path and checks if it is dividable by 2k-2.
    Returns the image in a `BmpImage` struct.
    On error `NULL` is returned and `errno` is set.
    if `errno` = `EINVAL` then the image is not a valid bmp image.
*/
BmpImage *parse_bmp(const char *path, uint8_t k);

/*
    Outputs the BmpImage into a file
*/
int output_bmp(BmpImage *bmp, const char *path);

/*
    Frees the `BmpImage` struct
*/
void free_bmp(BmpImage *bmp);

void save_shadow_number(BmpImage *bmp, uint8_t shadow_number);

uint8_t get_shadow_number(BmpImage *bmp);
