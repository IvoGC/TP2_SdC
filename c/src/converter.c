#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <string.h>
#include "../include/cdecl.h"
#include "../include/converter.h"

int PRE_CDECL asm_mult (int, int) POST_CDECL;

static int mult(int operator1, int operator2);
static int apiRequest(char* pricesString);
static Prices decodString(char* pricesString);
static Prices getPrices(int toConvert, Prices prices);
static void printResults(int toConvert, Prices prices, Prices results);

int main(int argc, char const *argv[])
{   
    while(1) {
        int toConvert = 0;
        printf("Enter a value to be converted: ");
        scanf("%d", &toConvert);
        if(toConvert == -1)
            break;
        char pricesString[128];
        if(apiRequest(pricesString) < 0)
            exit(EXIT_FAILURE);
        
        Prices prices;
        prices = decodString(pricesString);

        Prices results;
        results = getPrices(toConvert, prices);
        
        printResults(toConvert, prices, results);
    }

    return 0;
}

static void printResults(int toConvert, Prices prices, Prices results) {
    printf("-----------------------------------\n");
    printf("%d BTC:\n", toConvert);
    printf("     %d USD\n", results.usd);
    printf("     %d EUR\n", results.eur);
    printf("     %d ARS\n\n", results.ars);
    printf("Prices: %d USD, %d EUR, %d ARS\n", prices.usd, prices.eur, prices.ars);
    printf("-----------------------------------\n");
}


static Prices decodString(char* pricesString) {
    char* price = strtok(pricesString, ",");
    int p[3];
    int i=0;
    while (price != NULL){
        p[i++] = atoi(price);
        price = strtok(NULL, ",");
    }
    
    Prices prices;
    prices.usd =  p[0];
    prices.eur =  p[1];
    prices.ars =  p[2];
    
    return prices;
}

static int apiRequest(char* pricesString) {
    char string[128];
    pid_t pid;
    int pipeFd[2];

    pipe(pipeFd);
    pid = fork();

    if(pid < 0) {
        perror("Fork");
        return -1;
    }

    if(pid == 0) {
        close(pipeFd[0]);
        dup2(pipeFd[1], STDOUT_FILENO);
        close(pipeFd[1]);
        char* args[] = {"./bin/apiRequest", NULL};
        execvp(args[0], args);
        perror("execv");
        return -1;
    }
    else {
        close(pipeFd[1]);
        waitpid(pid, 0, 0);
        ssize_t size = read(pipeFd[0], string, sizeof(string));
        if(size < 0) {
            perror("read");
            return -1;
        }
        close(pipeFd[0]);
    }
    strcpy(pricesString, string);
    return 0;
}


static int mult(int operator1, int operator2) {
    int res = operator1*operator2;
    return res;
}

static Prices getPrices(int toConvert, Prices prices) {

    Prices results;
    results.usd = asm_mult(prices.usd, toConvert);
    results.eur = asm_mult(prices.eur, toConvert);
    results.ars = asm_mult(prices.ars, toConvert);

    return results;
}