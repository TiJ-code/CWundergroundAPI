#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wunderground.h"

int main(void) {
    if (wu_global_init() != 0) {
        fprintf(stderr, "Failed to initialize curl\n");
        return 1;
    }

    wu_client_t *client = wu_client_new_from_file("../api_key");
    if (!client) {
        fprintf(stderr, "Failed to create client\n");
        wu_global_cleanup();
        return 1;
    }

    char *result = wu_fetch_current_conditions(client, "Berlin,DE");
    if (result) {
        printf("Weather response:\n%s\n", result);
        free(result);
    } else {
        fprintf(stderr, "Failed to get current conditions\n");
    }

    wu_client_free(client);
    wu_global_cleanup();

    return 0;
}