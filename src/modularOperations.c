#include "include/modularOperations.h"
#include <math.h>

uint32_t modAdd(uint32_t val1, uint32_t val2, uint32_t mod){
    return (val1 + val2) % mod;
}

uint32_t modSub(uint32_t val1, uint32_t val2, uint32_t mod){
    return (val1-val2) % mod;
}

uint32_t modProd(uint32_t val1, uint32_t val2, uint32_t mod){
    return (val1*val2) % mod;
}

uint32_t modDiv(uint32_t val1, uint32_t val2, uint32_t mod){
    // val2 = denominador
    return (val1 * getInverse251(val2)) % mod;
}

uint32_t modPower(uint32_t val, uint32_t power, uint32_t mod){
    return ( (uint32_t) pow(val, power) ) % mod;
}

uint8_t getInverse251(uint32_t val){
    return inv251[val];
}