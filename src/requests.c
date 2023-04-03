#include "../include/requests.h"

Prices getPrices(float toConvert, float priceUsd, float priceEur, float priceArs) {

    Prices prices;
    prices.usd=toConvert*priceUsd;
    prices.euros=toConvert*priceEur;
    prices.pesos=toConvert*priceArs;

    return prices;
}
