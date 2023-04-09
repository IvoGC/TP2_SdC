#include <stdio.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata); 

int main(void) {
    CURL *curl;
    CURLcode res;
    char *url = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd,eur,ars";
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return 0;
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    cJSON *root = cJSON_Parse(ptr);
    cJSON *usd_value = cJSON_GetObjectItemCaseSensitive(root, "bitcoin");
    usd_value = cJSON_GetObjectItemCaseSensitive(usd_value, "usd");
    int usd = usd_value->valuedouble;
    cJSON *eur_value = cJSON_GetObjectItemCaseSensitive(root, "bitcoin");
    eur_value = cJSON_GetObjectItemCaseSensitive(eur_value, "eur");
    int eur = eur_value->valuedouble;
    cJSON *ars_value = cJSON_GetObjectItemCaseSensitive(root, "bitcoin");
    ars_value = cJSON_GetObjectItemCaseSensitive(ars_value, "ars");
    int ars = ars_value->valuedouble;
    printf("%d,%d,%d", usd, eur, ars);
    cJSON_Delete(root);
    return size * nmemb;
}