#pragma once
#include "bmp.h"
#include <stdint.h>

enum StegMode{LSB2, LSB4};

int hideShadowByes(BmpImage img, uint8_t * shadow);