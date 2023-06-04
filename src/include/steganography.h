#pragma once
#include "bmp.h"
#include "shadow.h"
#include <stdint.h>

enum StegMode { LSB2, LSB4 };

int hideShadowBytes(BmpImage *img, Shadow *shadow, enum StegMode mode);
void setLSB2(uint8_t *byte, uint8_t bit1, uint8_t bit0);
void setLSB4(uint8_t *byte, uint8_t bit3, uint8_t bit2, uint8_t bit1,
             uint8_t bit0);
