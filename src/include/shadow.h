#pragma once
#include <stdint.h>
#include <stdio.h>

struct v_ij {
    uint8_t m;
    uint8_t d;
};

typedef struct Shadow {
    uint8_t *bytes;
    size_t size;
    size_t idx;
} Shadow;

typedef struct v_ij v_ij;

Shadow *image_processing(uint8_t *image, size_t len, size_t k, size_t n);
v_ij *block_processing(uint8_t *block, size_t k, size_t n);
uint8_t polym(uint8_t *block, int val, size_t k);
void free_shadows(Shadow *shadows, size_t shadow_count);
uint8_t * recover_secret(Shadow *shadows, size_t k); 
