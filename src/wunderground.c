#define CURL_STATICLIB
#include <curl/curl.h>

#include "../include/wunderground.h"

int wu_global_init(void) {
    return curl_global_init(CURL_GLOBAL_DEFAULT);
}

void wu_global_cleanup(void) {
    curl_global_cleanup();
}