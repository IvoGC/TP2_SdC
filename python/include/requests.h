#ifndef REQUESTS_H
#define REQUESTS_H

typedef struct prices{
    float usd;
    float euros;
    float pesos;
} Prices;

Prices getPrices(float toConvert, float priceUsd, float priceEur, float priceArs);

#endif /*REQUESTS_H*/