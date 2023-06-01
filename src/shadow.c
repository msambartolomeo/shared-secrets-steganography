#include "shadow.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

int block_size;
int k = 4;
int n = 8;

/*
int main(void) {
    //acá recibo la imagen .bmp procesada, como cadena de bytes
    uint8_t image[] = {0xA0, 0xB1, 0x05, 0x91, 0xAA, 0xFF, 0x01, 0x03, 0x03, 0x01, 0xFF, 0x01, 0x03, 0x03, 0x01, 0x02, 0x03, 0xFF, 0xA0, 0xB1, 0x05, 0x91, 0xAA, 0xFF, 0x01, 0x03, 0x03, 0x01, 0xFF, 0x01, 0x03, 0x03, 0x01, 0x02, 0x03, 0xFF, 0xA0, 0xB1, 0x05, 0x91, 0xAA, 0xFF, 0x01, 0x03, 0x03, 0x01, 0xFF, 0x01, 0x03, 0x03, 0x01, 0x02, 0x03, 0xFF, 0xA0, 0xB1, 0x05, 0x91, 0xAA, 0xFF, 0x01, 0x03, 0x03, 0x01, 0xFF, 0x01, 0x03, 0x03, 0x01, 0x02, 0x03, 0xFF, 0xA0, 0xB1, 0x05, 0x91, 0xAA, 0xFF, 0x01, 0x03, 0x03, 0x01, 0xFF, 0x01, 0x03, 0x03, 0x01, 0x02, 0x03, 0xFF, 0xA0, 0xB1, 0x05, 0x91, 0xAA, 0xFF, 0x01, 0x03, 0x03, 0x01, 0xFF, 0x01, 0x03, 0x03, 0x01, 0x02, 0x03, 0xFF};
    int cant = sizeof(image)/sizeof(image[0]);
    uint8_t** shadows = image_processing(image, cant);
} */

uint8_t** image_processing(uint8_t *image, int cant) {

    //recibo la imagen (array de uint8_t) y su tamaño
    
    block_size = 2*k-2;

    if(k < 2 || k > 8 || (cant%block_size!=0)) {
        printf("ERROR EN K");
        return NULL;
    }

    //divido la imagen en bloques

    uint8_t** blocks = malloc(cant*sizeof(uint8_t));
    for(int i=0; i<cant; i+=block_size) {
        blocks[i/block_size] = malloc(block_size*sizeof(uint8_t));
        strncpy(blocks[i/block_size], image, block_size);
        image += block_size;
    }

    int t = cant/block_size;

    //proceso los bloques y genero las "sub-shadows"

    v_ij** vs = malloc(t*n*sizeof(v_ij));

    for(int i=0; i<t; i++) {
        vs[i] = block_processing(blocks[i]);
        /*printf("vs %d:\n", i);
        for(int l=0; l<n; l++)
            printf("%d %d ", vs[i][l].m, vs[i][l].d);
        printf("\n");*/
    }

    //genero las shadows a partir de las sub-shadows

    uint8_t** S = malloc(2*t*n*sizeof(uint8_t));

    for(int j=0; j<n; j++) {
        int pos = 0;
        S[j] = malloc(2*t*sizeof(uint8_t));
        //printf("\nS[%d]:\n", j);
        for(int v=0; v<t; v++) {
            S[j][pos++] = vs[v][j].m;
            S[j][pos++] = vs[v][j].d;
            //printf("%d %d ", S[j][pos-2], S[j][pos-1]);
        }
    }

    return S;
     
}

v_ij* block_processing(uint8_t *block) {

    
    //recibo un bloque de 2k-2 posiciones
    uint8_t *aux = malloc(k*sizeof(uint8_t));
    uint8_t *aux2 = malloc(k*sizeof(uint8_t));

    //TODO: poner b0 y b1 en aux2
    *aux2 = 0x01;
    *(aux2+1) = 0x02; //valores dummy

    strncpy(aux, block, k);
    strncpy(aux2+2, block+k, k-2);

    v_ij* v_i = malloc(n*sizeof(v_ij));

    //usando los polinomios generados a partir de los bloques, generamos la sub-shadow

    for(int j=0; j<n; j++) {
        v_i[j].m = (uint8_t)polym(aux, j+1);
        v_i[j].d = (uint8_t)polym(aux2, j+1);
    }

    return v_i;

}

uint8_t polym(uint8_t *block, int val) {
    int result = 0;
    int exp;
    for(int i=0; i<k; i++) {
        exp = pow(val, i);
        result += (int)block[i]*exp;
    }
    return result%251;
}




