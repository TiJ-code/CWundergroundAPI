#define CURL_STATICLIB
#include <curl/curl.h>

#include "../include/wunderground.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct memory {
    char* response;
    size_t size;
};

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct memory *mem = (struct memory *)userp;

    char *ptr = realloc(mem->response, mem->size + realsize + 1);
    if (!ptr) return 0;

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

static char* wu_http_get(const char *url) {
    CURL *curl = curl_easy_init();
    if (!curl) return NULL;

    struct memory chunk = {0};
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        free(chunk.response);
        return NULL;
    }

    return chunk.response;
}

char *wu_get_current_conditions(wu_client_t *client, const char *location) {
    if (!client || !location) return NULL;

    char url[512];
    snprintf(url, sizeof(url),
        "%s/conditions/q/%s.json?apiKey=%s",
        client->base_url, location, client->api_key);

    return wu_http_get(url);
}

int wu_global_init(void) {
    return curl_global_init(CURL_GLOBAL_DEFAULT);
}

void wu_global_cleanup(void) {
    curl_global_cleanup();
}

wu_client_t* wu_client_new(const char* api_key) {
    wu_client_t *client = malloc(sizeof(wu_client_t));
    if(!client) return NULL;

    client->api_key = strdup(api_key);
    if (!client->api_key) {
        free(client);
        return NULL;
    }

    const char* base = "https://api.weather.com/";
    client->base_url = strdup(base);
    if (!client->base_url) {
        free((char *)client->api_key);
        free(client);
        return NULL;
    }

    return client;
}

void wu_client_free(wu_client_t *client) {
    if (!client) return;

    free((char *)client->api_key);
    free((char *)client->base_url);
    free(client);
}