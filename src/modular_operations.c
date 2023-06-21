#include "include/modular_operations.h"
#include <math.h>

uint32_t mod_add(uint32_t val1, uint32_t val2, uint32_t mod) {
    return (val1 + val2) % mod;
}

uint32_t mod_sub(uint32_t val1, uint32_t val2, uint32_t mod) {
    int32_t sub = (val1 - val2);
    int32_t result = sub % (int32_t)mod;
    if (val1 < val2) {
        result += mod;
    }
    return (uint32_t)result;
}

uint32_t mod_prod(uint32_t val1, uint32_t val2, uint32_t mod) {
    return (val1 * val2) % mod;
}

uint32_t mod_div(uint32_t val1, uint32_t val2, uint32_t mod) {
    // val2 = denominador
    return (val1 * get_mod_inverse(val2)) % mod;
}

uint32_t mod_power(uint32_t val, uint32_t power, uint32_t mod) {
    return ((uint32_t)pow(val, power)) % mod;
}

uint8_t get_mod_inverse(uint32_t val) { return inv251[val]; }
