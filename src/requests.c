#include "../include/requests.h"
#include "../include/cdecl.h"

float PRE_CDECL asm_mult(float, float) POST_CDECL;

static float mult(float operator1, float operator2) {
    float res;
    res = asm_mult(operator1, operator2);
    return res;
}

Prices getPrices(float toConvert, float priceUsd, float priceEur, float priceArs) {

    Prices prices;
    prices.usd = mult(toConvert, priceUsd);
    prices.euros = mult(toConvert, priceEur);
    prices.pesos = mult(toConvert, priceArs);

    return prices;
}
