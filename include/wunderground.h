#ifndef CWUNDERGROUNDAPI_WUNDERGROUND_H
#define CWUNDERGROUNDAPI_WUNDERGROUND_H

#include "wunderground_units.h"

typedef struct {
    const char *api_key;
    wu_unit_t units;
    const char *language;
    const char *language_variant;
} wu_client_t;

/* --- Initialisation --- */
int wu_global_init(void);
void wu_global_cleanup(void);

/* --- Client management --- */
wu_client_t *wu_client_new(const char* api_key);
wu_client_t *wu_client_new_ex(const char* api_key, wu_unit_t units, const char *language, const char *language_variant);
wu_client_t *wu_client_new_from_file(const char* file_name);
wu_client_t *wu_client_new_from_file_ex(const char* file_name, wu_unit_t units, const char *language, const char *language_variant);
void wu_client_free(wu_client_t *client);

/* --- RAW API --- */
char *wu_fetch_current_conditions(wu_client_t *client, const char *location);

#endif //CWUNDERGROUNDAPI_WUNDERGROUND_H