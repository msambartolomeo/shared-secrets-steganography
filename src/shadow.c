#include "shadow.h"
#include "modularOperations.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

Shadow *image_processing(uint8_t *image, size_t len, size_t k, size_t n) {
    // recibo la imagen (array de uint8_t) y su tamaño
    int block_size = 2 * k - 2;

    if (k < 2 || k > 8 || (len % block_size != 0)) {
        printf("ERROR EN K");
        return NULL;
    }

    int t = len / block_size;

    // proceso los bloques y genero las "sub-shadows"
    v_ij **vs = malloc(t * sizeof(v_ij *));

    srand(time(NULL));
    for (int i = 0; i < t; i++) {
        vs[i] = block_processing(&image[i * block_size], k, n);
    }

    // genero las shadows a partir de las sub-shadows
    Shadow *shadows = malloc(n * sizeof(Shadow));
    size_t shadow_size = 2 * t;

    for (size_t j = 0; j < n; j++) {
        int pos = 0;
        shadows[j].bytes = malloc(shadow_size * sizeof(uint8_t));
        shadows[j].size = shadow_size;
        for (int v = 0; v < t; v++) {
            shadows[j].bytes[pos++] = vs[v][j].m;
            shadows[j].bytes[pos++] = vs[v][j].d;
        }
        shadows[j].idx = j;
    }

    for (int i = 0; i < t; i++) {
        free(vs[i]);
    }
    free(vs);

    return shadows;
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

void free_shadows(Shadow *shadows, size_t shadow_count) {
    for (size_t i = 0; i < shadow_count; i++) {
        free(shadows[i].bytes);
    }
    free(shadows);
}

uint8_t * lagrange(uint8_t *vs, size_t k, size_t *idxs) {

}

uint8_t * recover_secret(Shadow * shadows, size_t k) {

    size_t block_count = shadows[0].size / 2; 
    size_t* shadow_idxs = malloc(sizeof(size_t) * k);

    for(size_t i=0; i<k; i++) {
        shadow_idxs[i] = shadows[i].idx;
    }

    uint8_t* secret = malloc(sizeof(uint8_t) * block_count * k);

    size_t bytes = 0;

    for(size_t i=0; i<block_count; i++) {
        //para cada bloque, tengo que agarrar 2 bytes de cada sombra
        v_ij* vs = malloc(sizeof(v_ij) * k);
        for(size_t j=0; j<k; j++) {
            vs[j].m = shadows[j].bytes[i*2];
            vs[j].d = shadows[j].bytes[(i*2)+1];
        }
        //con esos dos bytes de cada sombra reconstruyo los polinomios y obtengo los bytes del bloque
        uint8_t *coeffs = lagrange(vs, k, shadow_idxs);
        for(size_t j=0; j<k; j++) {
            secret[bytes+j] = coeffs[j];
        }
        bytes += k;
    }

    return secret;


}
