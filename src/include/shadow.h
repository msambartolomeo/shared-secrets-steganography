#pragma once
#include <stdint.h>
#include <stdio.h>

struct v_ij {
    uint8_t m;
    uint8_t d;
};

typedef struct v_ij v_ij;

uint8_t **image_processing(uint8_t *image, int cant, size_t k, size_t n);
v_ij *block_processing(uint8_t *block, size_t k, size_t n);
uint8_t polym(uint8_t *block, int val, size_t k);