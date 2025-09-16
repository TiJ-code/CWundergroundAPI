#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wunderground.h"
#include "wunderground_units.h"

int main(void) {
    if (wu_global_init() != 0) {
        fprintf(stderr, "wu_global_init failed\n");
        return 1;
    }

    wu_client_t *client = wu_client_new_from_file_ex("../api_key", WU_UNIT_METRIC, NULL, NULL);
    if (!client) {
        fprintf(stderr, "wu_client_new_from_file_ex failed\n");
        wu_global_cleanup();
        return 1;
    }

    char *json = wu_fetch_current_conditions(client);
    if (!json) {
        fprintf(stderr, "wu_fetch_current_conditions failed\n");
    } else {
        printf("Current weather JSON for %s:\n%s\n", location, json);
        free(json);
    }

    wu_client_free(client);
    wu_global_cleanup();

    return 0;
}