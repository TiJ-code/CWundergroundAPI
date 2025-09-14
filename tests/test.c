#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wunderground.h"

static char *read_api_key(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("fopen");
        return NULL;
    }

    char buffer[256];
    if (!fgets(buffer, sizeof(buffer), f)) {
        fclose(f);
        return NULL;
    }

    fclose(f);

    buffer[strcspn(buffer, "\r\n")] = '\0';

    return strdup(buffer);
}

int main(void) {
    if (wu_global_init() != 0) {
        fprintf(stderr, "Failed to initialize curl\n");
        return 1;
    }

    const char *api_key = read_api_key("../api_key");
    wu_client_t *client = wu_client_new(api_key);
    if (!client) {
        fprintf(stderr, "Failed to create client\n");
        wu_global_cleanup();
        return 1;
    }

    char *result = wu_get_current_conditions(client, "Berlin,DE");
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