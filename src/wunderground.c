#define CURL_STATICLIB
#include <curl/curl.h>

#include "../include/wunderground.h"

#include <stdlib.h>

int wu_global_init(void) {
    return curl_global_init(CURL_GLOBAL_DEFAULT);
}

void wu_global_cleanup(void) {
    curl_global_cleanup();
}

wu_client_t* wu_client_new(const char* api_key) {
    wu_client_t* client = (wu_client_t*) malloc(sizeof(wu_client_t));
    if (!client) return NULL;

    client->api_key = api_key;
    client->base_url = "https://wunderground.com/api/";

    return client;
}

void wu_client_free(wu_client_t* client) {
    if (!client) return;

    free(client->api_key);
    free(client->base_url);
    free(client);
}