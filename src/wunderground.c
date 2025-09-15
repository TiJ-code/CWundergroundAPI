#define CURL_STATICLIB
#include <curl/curl.h>

#include <json-c/json.h>

#include "../include/wunderground.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        free(buffer.data);
        return NULL;
    }

    return buffer.data; // caller takes ownership
}

/**
 * @brief Query the current weather conditions from Wunderground.
 *
 * This builds a Wunderground API URL for the "conditions" endpoint and
 * downloads the resulting JSON data as a string.
 *
 * @param client Pointer to a wu_client_t object representing the API client
 * @param location String specifying the location to query.
 *                 Can be either "latitude,longitude" (e.g. "52.52,13.41")
 *                 or "Country/City" (e.g. "DE/Berlin")
 *
 * @return Pointer to a dynamically allocated JSON response string.
 *         The caller must free() this buffer when done.
 *         Returns NULL if the request fails.
 */
char *wu_fetch_current_conditions(wu_client_t *client, const char *location) {
    if (!client || !location) return NULL;

    char url[512];
    snprintf(url, sizeof(url),
        "%s/api/%s/conditions/q/%s.json",
        client->base_url, client->api_key, "52.52,13.41");

    return http_get(url); // caller frees
}

/* --- Library setup / teardown --- */

/**
 * @brief Initialise the Wunderground client library.
 *
 * This must be called before using any Wunderground client functions.
 * Internally, it initialises the libcurl global state.
 *
 * @return Zero on success, non-zero if initialisation failed.
 */
int wu_global_init(void) {
    return curl_global_init(CURL_GLOBAL_DEFAULT);
}

/**
 * @brief Clean up global resources used by the Wunderground client library.
 *
 * This should be called once, after all Wunderground API usage is finished.
 * It releases resources used by libcurl.
 */
void wu_global_cleanup(void) {
    curl_global_cleanup();
}

/* --- Client management --- */

/**
 * @brief Create a new Wunderground API client.
 *
 * Allocates and initialises a wu_client_t structure using the given API key.
 * The base URL is set to "https://api.wunderground.com/" by default.
 *
 * @param api_key Null-terminated string containing the Wunderground API key.
 *
 * @return Pointer to a new wu_client_t object, or NULL if allocation fails.
 *         The returned client must be released with wu_client_free().
 */
wu_client_t* wu_client_new(const char* api_key) {
    wu_client_t *client = malloc(sizeof(wu_client_t));
    if(!client) return NULL;

    client->api_key = strdup(api_key);
    if (!client->api_key) {
        free(client);
        return NULL;
    }

    const char *base = "https://api.wunderground.com/";
    client->base_url = strdup(base);
    if (!client->base_url) {
        free((char *)client->api_key);
        free(client);
        return NULL;
    }

    return client;
}


/**
 * @brief Create a new Wunderground API client from a file.
 *
 * Reads an API key from the given file and constructs a client.
 * The file should contain the API key on the first line.
 *
 * @param filename Path to a file containing the API key.
 *
 * @return Pointer to a new wu_client_t object, or NULL on failure.
 *         The returned client must be released with wu_client_free().
 */
wu_client_t* wu_client_new_from_file(const char *filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return NULL;

    char buffer[256];
    if (!fgets(buffer, sizeof(buffer), file)) return NULL;

    buffer[strcspn(buffer, "\r\n")] = 0; // strip newline

    wu_client_t *client = wu_client_new(buffer);
    fclose(file);

    return client;
}

/**
 * @brief Free a Wunderground API client.
 *
 * Releases all memory associated with a wu_client_t object.
 *
 * @param client Pointer to the wu_client_t object to free.
 *               Passing NULL has no effect.
 */
void wu_client_free(wu_client_t *client) {
    if (!client) return;

    free((char *)client->api_key);
    free((char *)client->base_url);
    free(client);
}