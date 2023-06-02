#include "include/steganography.h"
#include "include/bmp.h"
#include <stdint.h>

int hideShadowBytes(BmpImage img, uint8_t * shadow, StegMode mode){
    if(img == NULL || shadow == NULL){
        return -1;
    }

    uint8_t * imgBytes = img->image;
    int i = 0;
    int k = 0;
    for(/*byte : shadow*/){ //TODO: Escribir esta condición
        int * bits = getBitArray(*(shadow + i));
        if(mode == LSB2){
            for(int j = 0; j < 6; j+=2){
                setLSB2(imgBytes + (k++), bits[j], bits[j+1]); // Pone en últimos 2 bits bits[j] y bits[j+1]
            }
        }
        if(mode == LSB4){
            setLSB4(imgBytes + (k++), bits[0], bits[1], bits[2], bits[3]);
            setLSB4(imgBytes + (k++), bits[4], bits[5], bits[6], bits[7]);
        }
    }

    /* Acá falta, teniendo el arreglo de bytes cambiado, guardarlo en el archivo original
    imagino que es un memcpy pero por las dudas no quiero meter la pata*/
    
    return 0;
}

void setLSB2(uint8_t * byte, uint8_t bit1, uint8_t bit0){

}

void setLSB4(uint8_t * byte, uint8_t bit3, uint8_t bit2, uint8_t bit1, uint8_t bit0){

}

int * getBitArray(uint8_t byte){
    int bits[8];
    for (int i = 7; i >= 0; i--) {
        bits[i] = (byte >> i) & 1;
    }
    return bits;
}