#include "shadow.h"
#include "modularOperations.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
int main(void) {
    //acá recibo la imagen .bmp procesada, como cadena de bytes
    uint8_t image[] = {0xA0, 0xB1, 0x05, 0x91, 0xAA, 0xFF, 0x01, 0x03, 0x03,
0x01, 0xFF, 0x01, 0x03, 0x03, 0x01, 0x02, 0x03, 0xFF, 0xA0, 0xB1, 0x05, 0x91,
0xAA, 0xFF, 0x01, 0x03, 0x03, 0x01, 0xFF, 0x01, 0x03, 0x03, 0x01, 0x02, 0x03,
0xFF, 0xA0, 0xB1, 0x05, 0x91, 0xAA, 0xFF, 0x01, 0x03, 0x03, 0x01, 0xFF, 0x01,
0x03, 0x03, 0x01, 0x02, 0x03, 0xFF, 0xA0, 0xB1, 0x05, 0x91, 0xAA, 0xFF, 0x01,
0x03, 0x03, 0x01, 0xFF, 0x01, 0x03, 0x03, 0x01, 0x02, 0x03, 0xFF, 0xA0, 0xB1,
0x05, 0x91, 0xAA, 0xFF, 0x01, 0x03, 0x03, 0x01, 0xFF, 0x01, 0x03, 0x03, 0x01,
0x02, 0x03, 0xFF, 0xA0, 0xB1, 0x05, 0x91, 0xAA, 0xFF, 0x01, 0x03, 0x03, 0x01,
0xFF, 0x01, 0x03, 0x03, 0x01, 0x02, 0x03, 0xFF}; int cant =
sizeof(image)/sizeof(image[0]); uint8_t** shadows = image_processing(image,
cant);
} */

void free_blocks(void **blocks, int t) {
    for (int i = 0; i < t; i++) {
        free(blocks[i]);
    }
    free(blocks);
}

uint8_t **image_processing(uint8_t *image, int cant, size_t k, size_t n) {

    // recibo la imagen (array de uint8_t) y su tamaño

    int block_size = 2 * k - 2;

    if (k < 2 || k > 8 || (cant % block_size != 0)) {
        printf("ERROR EN K");
        return NULL;
    }

    // divido la imagen en bloques

    uint8_t **blocks = malloc(cant * sizeof(uint8_t));
    for (int i = 0; i < cant; i += block_size) {
        blocks[i / block_size] = malloc(block_size * sizeof(uint8_t));
        memcpy(blocks[i / block_size], image, block_size);
        image += block_size;
    }

    int t = cant / block_size;

    // proceso los bloques y genero las "sub-shadows"

    v_ij **vs = malloc(t * n * sizeof(v_ij));

    srand(time(NULL));

    for (int i = 0; i < t; i++) {
        vs[i] = block_processing(blocks[i], k, n);
        /*printf("vs %d:\n", i);
        for(int l=0; l<n; l++)
            printf("%d %d ", vs[i][l].m, vs[i][l].d);
        printf("\n");*/
    }

    free_blocks((void **)blocks, t);

    // genero las shadows a partir de las sub-shadows

    uint8_t **S = malloc(t * n * sizeof(v_ij));

    for (size_t j = 0; j < n; j++) {
        int pos = 0;
        S[j] = malloc(2 * t * sizeof(uint8_t));
        // printf("\nS[%d]:\n", j);
        for (int v = 0; v < t; v++) {
            S[j][pos++] = vs[v][j].m;
            S[j][pos++] = vs[v][j].d;
            // printf("%d %d ", S[j][pos-2], S[j][pos-1]);
        }
    }

    free_blocks((void **)vs, t);

    return S;
}

v_ij *block_processing(uint8_t *block, size_t k, size_t n) {

    // recibo un bloque de 2k-2 posiciones
    uint8_t *a_is = malloc(k * sizeof(uint8_t));
    uint8_t *b_is = malloc(k * sizeof(uint8_t));

    uint8_t r = 0;
    while (r == 0) {
        r = rand() % 251;
    }

    // r * ai,0 + bi,0 = 0 && r * ai,1 + bi,1 == 0 (todo mod251)
    uint8_t ai0 = *block == 0 ? 1 : *block;
    uint8_t ai1 = *(block + 1) == 0 ? 1 : *(block + 1);

    uint8_t bi0 = MODULE - modProd(r, ai0, MODULE);
    uint8_t bi1 = MODULE - modProd(r, ai1, MODULE);

    *b_is = bi0;
    *(b_is + 1) = bi1;

    memcpy(a_is, block, k);
    memcpy(b_is + 2, block + k, k - 2);

    v_ij *v_i = malloc(n * sizeof(v_ij));

    // usando los polinomios generados a partir de los bloques, generamos la
    // sub-shadow

    for (size_t j = 0; j < n; j++) {
        v_i[j].m = polym(a_is, j + 1, k);
        v_i[j].d = polym(b_is, j + 1, k);
    }

    free(a_is);
    free(b_is);

    return v_i;
}

uint8_t polym(uint8_t *block, int val, size_t k) {
    int result = 0;
    int exp;
    for (size_t i = 0; i < k; i++) {
        exp = modPower(val, i, MODULE);
        result += modProd(block[i], exp, MODULE);
    }
    return result % 251;
}
