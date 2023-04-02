#ifndef REQUESTS_H
#define REQUESTS_H

typedef struct prices{
    float usd;
    float euros;
    float pesos;
} Prices;

Prices getPrices();

#endif /*REQUESTS_H*/