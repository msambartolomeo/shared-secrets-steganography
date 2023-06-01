#include "include/distribute.h"
#include "bmp.h"
#include "shadow.h"
#include <stdio.h>
#include <stdlib.h>

void distribute(char *filename, int k, int n, char *directory) {
    BmpImage *image = parse_bmp(filename, k);
    if (image == NULL) {
        fprintf(stderr, "Invalid image\n");
        exit(EXIT_FAILURE);
    }
    uint8_t **shadows = image_processing(image->image, image->image_size, k, n);
    if (shadows == NULL) {
        fprintf(stderr, "for compilation\n");
    }
}