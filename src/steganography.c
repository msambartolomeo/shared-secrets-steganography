#include "include/steganography.h"
#include "include/bmp.h"
#include <stdint.h>
#include <stdlib.h>

int hideShadowBytes(BmpImage *img, uint8_t *shadow, enum StegMode mode) {
    if (img == NULL || shadow == NULL) {
        return -1;
    }

    uint8_t *imgBytes = img->image;
    int k = 0;
    for (long unsigned i = 0; i < sizeof(shadow);
         i++) { // TODO: Chequear esta condición
        int *bits = getBitArray(*(shadow + i));
        if (mode == LSB2) {
            for (int j = 0; j <= 6; j += 2) {
                setLSB2(
                    imgBytes + (k++), bits[j],
                    bits[j + 1]); // Pone en últimos 2 bits bits[j] y bits[j+1]
            }
        }
        if (mode == LSB4) {
            setLSB4(imgBytes + (k++), bits[0], bits[1], bits[2], bits[3]);
            setLSB4(imgBytes + (k++), bits[4], bits[5], bits[6], bits[7]);
        }
    }
    return 0;
}

void setLSB2(uint8_t *byte, uint8_t bit1, uint8_t bit0) {
    unsigned char toInclude = 0x00;

    toInclude |= (bit1 & 0x01) << 1; // 0000 00 0/1 0
    toInclude |= (bit0 & 0x01);      // 0000 00x0/1

    *(byte) &= 0xFC;
    *(byte) |= toInclude;
}

void setLSB4(uint8_t *byte, uint8_t bit3, uint8_t bit2, uint8_t bit1,
             uint8_t bit0) {
    unsigned char toInclude = 0x00;

    toInclude |= (bit3 & 0x01) << 3;
    toInclude |= (bit2 & 0x01) << 2; // 0000 00 0/1 0
    toInclude |= (bit1 & 0x01) << 1; // 0000 00 0/1 0
    toInclude |= (bit0 & 0x01);

    *(byte) &= 0xF0;
    *(byte) |= toInclude;
}

int *getBitArray(uint8_t byte) {
    int *bits = (int *)malloc(sizeof(int) * 8);
    for (int i = 7; i >= 0; i--) {
        bits[i] = (byte >> i) & 1;
    }
    return bits;
}