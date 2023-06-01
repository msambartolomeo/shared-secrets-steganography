#include "include/distribute.h"
#include "bmp.h"
#include "shadow.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

void distribute(char *filename, int k, int n, char *directory) {
    BmpImage *secret_bmp = parse_bmp(filename, k);
    if (secret_bmp == NULL) {
        if (errno == EINVAL) {
            fprintf(stderr, "Secret image is not a valid bmp image");
        } else {
            perror("Error parsing secret image");
        }
        exit(EXIT_FAILURE);
    }
    uint8_t **shadows =
        image_processing(secret_bmp->image, secret_bmp->image_size, k, n);
    if (shadows == NULL) {
        fprintf(stderr, "for compilation\n");
    }

    free_bmp(secret_bmp);
}