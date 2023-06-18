#include "shadow.h"
#include "modularOperations.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int *factorize(int *factors, int size);
void getValue(int *factors, int *result, int n, int size, int index, int dim,
              int aux[]);
int *lagrange(v_ij *vs, int k, size_t *idxs);

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
    if(vs == NULL){
        perror("Malloc error");
        exit(1);
    }

    srand(time(NULL));
    for (int i = 0; i < t; i++) {
        vs[i] = block_processing(&image[i * block_size], k, n);
    }

    // genero las shadows a partir de las sub-shadows
    Shadow *shadows = malloc(n * sizeof(Shadow));
    if(shadows == NULL){
        perror("Malloc error");
        exit(1);
    }
    size_t shadow_size = 2 * t;

    for (size_t j = 0; j < n; j++) {
        int pos = 0;
        shadows[j].bytes = malloc(shadow_size * sizeof(uint8_t));
        if(shadows[j].bytes == NULL){
            perror("Malloc error");
            exit(1);
        }
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
    if(a_is == NULL){
        perror("Malloc error");
        exit(1);
    }
    uint8_t *b_is = malloc(k * sizeof(uint8_t));
    if(b_is == NULL){
        perror("Malloc error");
        exit(1);
    }

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
    if(v_i == NULL){
        perror("Malloc error");
        exit(1);
    }

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

uint8_t checkForCheating(int ai1, int ai0, int bi1, int bi0) {
    for (int i = 0; i < MODULE; i++) {
        if (modAdd(modProd(i, ai0, MODULE), bi0, MODULE) == 0 &&
            modAdd(modProd(i, ai1, MODULE), bi1, MODULE) == 0) {
            return 0;
        }
    }
    return 1;
}

int convertToPositive(int num) {
    while (num < 0) {
        num += MODULE;
    }
    return num;
}

int *lagrange(v_ij *vs, int k, size_t *idxs) {

    int size = 2 * k - 2;
    int *coeffs = calloc(size, sizeof(int));

    /*para cada punto de la interpolación, dado por idxs[i] y vs[i].m/d (para
    polinomio f/g), consigo los coeficientes y los sumo*/
    for (int i = 0; i < k; i++) {
        int *factors = malloc(sizeof(int) * (k - 1));
        if(factors == NULL){
            perror("Malloc error");
            exit(1);
        }

        // en cada v_ij tengo la componente y de cada punto de la interpolación,
        // para los polinomios f y g
        int yf = (int)vs[i].m;
        int yg = (int)vs[i].d;

        int num = 0;
        int div = 1;

        for (int j = 0; j < k; j++) {
            if (i != j) {
                div *= (idxs[i] - idxs[j]);
                factors[num++] = -(idxs[j]);
            }
        }

        if (div < 0) {
            div = (div % MODULE) + MODULE;
        }

        int multiplier_f = modDiv(yf, div, MODULE);
        int multiplier_g = modDiv(yg, div, MODULE);

        // ya tengo los multiplicadores (suponiendo estructura
        // y(x-a)(x-b)/(x0-a)(x0-b), entonces serían y/(x0-a)(x0-b) )

        int *current_coeffs_f = factorize(factors, k);

        // las primeras k posiciones las lleno usando los coeficientes del
        // polinomio f tengo que ir de atrás para adelante porque los
        // coeficientes se guardaron en el estilo ax^2 + bx + c
        for (int j = 0; j < k; j++) {
            current_coeffs_f[k - j - 1] *= multiplier_f;
            current_coeffs_f[k - j - 1] %= MODULE;
            if (current_coeffs_f[k - j - 1] < 0) {
                current_coeffs_f[k - j - 1] += MODULE;
            }

            coeffs[j] += current_coeffs_f[k - j - 1];
            coeffs[j] %= MODULE;
        }

        // ACA TENGO LOS COEFICIENTES DE F DESCENDIENTES

        // luego, agarro los k-2 restantes del polinomio g, ignorando los
        // últimos dos (los que multiplican a x^1 y x^0)

        int *current_coeffs_g = factorize(factors, k);

        for (int j = k; j < size; j++) {
            current_coeffs_g[size - j - 1] *= multiplier_g;
            current_coeffs_g[size - j - 1] %= MODULE;
            if (current_coeffs_g[size - j - 1] < 0) {
                current_coeffs_g[size - j - 1] += MODULE;
            }

            coeffs[j] += current_coeffs_g[size - j - 1];
            coeffs[j] %= MODULE;
        }

        int ai0 = convertToPositive(current_coeffs_f[k - 1]);
        int ai1 = convertToPositive(current_coeffs_f[k - 2]);
        int bi0 = convertToPositive(current_coeffs_g[k - 1]);
        int bi1 = convertToPositive(current_coeffs_g[k - 2]);

        if (checkForCheating(ai1, ai0, bi1, bi0) != 0) {
            exit(1);
        }

        free(factors);
        free(current_coeffs_f);
        free(current_coeffs_g);
    }

    return coeffs;
}

void getValue(int *factors, int *result, int n, int size, int index, int dim,
              int aux[]) {

    // función recursiva que obtiene todas las particiones de tamaño n para un
    // grupo de tamaño size

    // la suma de los productos de los miembros de estas particiones es el
    // coeficiente de la potencia (size-n) del polinomio

    if (dim == n) {
        int coeff = 1;
        for (int i = 0; i < n; i++) {
            coeff *= aux[i];
        }
        *result += coeff;
        return;
    }

    if (index >= size) {
        return;
    }

    aux[dim] = factors[index];

    getValue(factors, result, n, size, index + 1, dim + 1, aux);
    getValue(factors, result, n, size, index + 1, dim, aux);
}

int *factorize(int *factors, int size) {

    // acá tengo la expansión de (x-a)(x-b)(x-c)... y busco obtener algo en la
    // forma px^2 + qx + r para devolver p, q y r

    int *coeffs = calloc(size, sizeof(int));
    coeffs[0] = 1;

    for (int i = 1; i < size; i++) {
        int *aux = malloc(i * sizeof(int));
        if(aux == NULL){
            perror("Malloc error");
            exit(1);
        }
        getValue(factors, &coeffs[i], i, size - 1, 0, 0, aux);
        free(aux);
    }

    return coeffs;
}

Secret recover_secret(Shadow *shadows, size_t k) {

    // recibo las sombras, que tienen un v_ij (dos bytes -> m y d) por bloque
    // cada una

    size_t block_count = shadows[0].size / 2;
    size_t *shadow_idxs = malloc(sizeof(size_t) * k);
    if(shadow_idxs == NULL){
        perror("Malloc error");
        exit(1);
    }

    /*el nro de sombra es la componente x de los puntos para la interpolación,
    dado que las sombras están compuestas por el conjunto de p(x) para los
    polinomios de cada bloque*/

    for (size_t i = 0; i < k; i++) {
        shadow_idxs[i] = shadows[i].idx + 1;
    }

    size_t size = 2 * k - 2;
    uint8_t *secret = malloc(sizeof(uint8_t) * block_count * size);
    if(secret == NULL){
        perror("Malloc error");
        exit(1);
    }

    size_t bytes = 0;

    for (size_t i = 0; i < block_count; i++) {
        // para cada bloque, tengo que agarrar 2 bytes de cada sombra
        v_ij *vs = malloc(sizeof(v_ij) * k);
        if(vs == NULL){
            perror("Malloc error");
            exit(1);
        }
        for (size_t j = 0; j < k; j++) {
            vs[j].m = shadows[j].bytes[i * 2];
            vs[j].d = shadows[j].bytes[(i * 2) + 1];
        }
        // con esos dos bytes de cada sombra reconstruyo los polinomios y
        // obtengo los bytes del bloque
        int *coeffs = lagrange(vs, k, shadow_idxs);
        // printPolynomial(coeffs, k - 1);
        // voy construyendo el secreto
        for (size_t j = 0; j < size; j++) {
            secret[bytes + j] = (uint8_t)coeffs[j];
        }
        bytes += size;
        free(coeffs);
        free(vs);
    }

    free(shadow_idxs);

    Secret s = {.size = bytes, .bytes = secret};
    return s;
}

void printPolynomial(int *coefficients, int degree) {
    int i;

    // Print the coefficients in descending order
    for (i = degree; i >= 0; i--) {
        printf("%dx^%d ", coefficients[i], i);

        // Print a plus sign if there are more terms
        if (i > 0) {
            printf("+ ");
        }
    }

    printf("\n");
}