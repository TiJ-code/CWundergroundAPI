#define CURL_STATICLIB
#include <curl/curl.h>

#include <json-c/json.h>

#include "wunderground.h"
#include "wunderground_units_internal.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * @file wunderground.c
 * @brief Implementation of the Wunderground C API client.
 *
 * Provides HTTP-based access to the Weather Underground API using libcurl.
 * Includes client management, global init/cleanup, and raw data fetching.
 */

/// Base URL for Weather Underground API requests
const char *BASE_API_URL = "https://api.weather.com/v3/";

/* --- Initial HTTP Helpers --- */

/**
 * @struct http_buffer_t
 * @brief Buffer for storing HTTP response data in memory.
 *
 * This structure is dynamically resized by the write_callback function as
 * more data is received from the HTTP response. It is always null-terminated
 * so it can be treated as a regular C string.
 */
typedef struct {
    char *data;  /**< Pointer to dynamically allocated response data */
    size_t size; /**< Current size of the buffer in bytes */
} http_buffer_t;

/**
 * @brief Callback for libcurl to write received data into a dynamically growing buffer
 *
 * This function is called by libcurl whenever a chunk of data is received.
 * It appends the data to an internal buffer that grows dynamically.
 *
 * @param contents Pointer to the incoming data buffer from libcurl
 * @param size Size of each element (always 1 in this context).
 * @param nmemb Number of elements (total bytes = size * nmemb).
 * @param userp Pointer to the http_buffer_t structure used to store data.
 *
 * @return The number of bytes successfully handled. Returning a different
 *         number signals an error to libcurl and aborts the request.
 */
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t bytes = size * nmemb;
    http_buffer_t *buffer = (http_buffer_t *)userp;

    // Reallocate buffer to fit new data
    char *new_ptr = realloc(buffer->data, buffer->size + bytes + 1);
    if (!new_ptr) return 0; // out of memory, abort

    buffer->data = new_ptr;
    memcpy(buffer->data + buffer->size, contents, bytes);
    buffer->size += bytes;
    buffer->data[buffer->size] = '\0';

    return bytes;
}

/**
 * @brief Perform an HTTP GET request and return the response as a string.
 *
 * This functions uses libcurl to download the contents of the given URL
 * into a dynamically allocated string buffer.
 *
 * @param url Null-terminated string with the full URL to fetch.
 *
 * @return Pointer to a null-terminated string containing the response.
 *         The caller is responsible for freeing this buffer using free().
 *         Returns NULL if the request fails or if memory cannot be allocated.
 */
static char* http_get(const char *url) {
    CURL *curl = curl_easy_init();
    if (!curl) return NULL;

    http_buffer_t buffer = {0}; // empty buffer

    curl_easy_setopt(curl, CURLOPT_URL, url);

    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "CURL error: %s\n", curl_easy_strerror(res));
        free(buffer.data);
        return NULL;
    }

    curl_easy_cleanup(curl);
    return buffer.data; // caller takes ownership
}

/* --- Public API: Weather Data Fetch --- */

/**
 * @brief Fetch current weather conditions for a location.
 *
 * Builds an API request to Wunderground and downloads the JSON result.
 *
 * @param client   Pointer to initialised wu_client_t
 * @param location String specifying location (e.g. "52.52,13.41" or "DE/Berlin")
 *
 * @return Pointer to JSON response string (caller must free), or NULL on error
 */
char *wu_fetch_current_conditions(wu_client_t *client, const char *location) {
    if (!client || !location) return NULL;

    char url[512];

    snprintf(url, sizeof(url),
        "%s/wx/observations/current?apiKey=%s&geocode=%s&format=json&units=%c&language=%s-%s",
        BASE_API_URL,
        client->api_key,
        location,
        wu_unit_to_char(client->units),
        client->language, client->language_variant
    );

    return http_get(url);
}

/* --- Library setup / teardown --- */

/**
 * @brief Initialise the Wunderground client library.
 *
 * Must be called before any wu_client_t usage.
 * Wraps curl_global_init().
 *
 * @return 0 on success, non-zero on failure
 */
int wu_global_init(void) {
    return curl_global_init(CURL_GLOBAL_DEFAULT);
}

/**
 * @brief Clean up global resources used by the client library.
 *
 * Should be called once at the end of program execution.
 */
void wu_global_cleanup(void) {
    curl_global_cleanup();
}

/* --- Client Management --- */

/**
 * @brief Create a new Wunderground client with default settings.
 *
 * Defaults: units=metric, language="en", variant="GB".
 *
 * @param api_key Wunderground API key
 * @return New client object, or NULL on allocation failure
 */
wu_client_t* wu_client_new(const char* api_key) {
    return wu_client_new_from_file_ex(api_key, WU_UNIT_METRIC, NULL, NULL);
}

/**
 * @brief Create a new Wunderground client with custom settings.
 *
 * @param api_key   Wunderground API key
 * @param units     Measurement system (metric/imperial/hybrid)
 * @param language  Language code (e.g. "en", "de")
 * @param language_variant Regional variant (e.g. "GB", "US")
 * @return New client object, or NULL on failure
 */
wu_client_t* wu_client_new_ex(const char* api_key,
                              const wu_unit_t units,
                              const char *language, const char *language_variant) {
    wu_client_t *client = malloc(sizeof(wu_client_t));
    if (!client) return NULL;

    client->api_key = strdup(api_key);
    if (!client->api_key) {
        free(client);
        return NULL;
    }

    client->units = units ? units : WU_UNIT_METRIC;
    client->language = language ? strdup(language) : "en";
    client->language_variant = language_variant ? strdup(language_variant) : "GB";

    return client;
}

/**
 * @brief Create a new client, reading the API key from a file.
 *
 * File must contain the API key on the first line.
 *
 * @param file_name Path to API key file
 * @return New client object, or NULL on failure
 */
wu_client_t* wu_client_new_from_file(const char *file_name) {
    return wu_client_new_from_file_ex(file_name, WU_UNIT_METRIC, NULL, NULL);
}

/**
 * @brief Create a new client from a file with custom settings.
 *
 * @param file_name       Path to API key file
 * @param units           Measurement system
 * @param language        Language code
 * @param language_variant Regional variant
 * @return New client object, or NULL on failure
 */
wu_client_t* wu_client_new_from_file_ex(const char *file_name,
                                        const wu_unit_t units,
                                        const char *language, const char *language_variant) {
    FILE* file = fopen(file_name, "r");
    if (!file) return NULL;

    char buffer[256];
    if (!fgets(buffer, sizeof(buffer), file)) {
        fclose(file);
        return NULL;
    }
    fclose(file);

    buffer[strcspn(buffer, "\r\n")] = 0; // strip newline

    return wu_client_new_ex(buffer, units, language, language_variant);;
}

/**
 * @brief Free a client and its resources.
 *
 * @param client Client to free (NULL-safe)
 */
void wu_client_free(wu_client_t *client) {
    if (!client) return;

    free((char *)client->api_key);
    free((char *)client->language);
    free((char *)client->language_variant);
    free(client);
}