#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <gnu/lib-names.h> 
#include "../include/requests.h"

int main(void) {
    Prices results;

    void *handle = dlopen("../lib/librequests.so", RTLD_LAZY);


    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    
    Prices (*getPrices)(float, float, float, float);

    getPrices = dlsym(handle, "getPrices");
    if(!getPrices) {
        fprintf(stderr, "Error al obtener el puntero a la función: %s\n", dlerror());
        exit(1);
    }

    results = getPrices(1.0f, 2.0f, 3.0f, 4.0f);
    printf("Prices: usd: %f, euros: %f, ars: %f",results.usd, results.euros, results.pesos);

    dlclose(handle);

    return 0;

}


