#ifndef CWUNDERGROUNDAPI_WUNDERGROUND_H
#define CWUNDERGROUNDAPI_WUNDERGROUND_H

typedef struct {
    const char* api_key;
    const char* base_url;
} wu_client_t;

/* --- Initialisation --- */
int wu_global_init(void);
void wu_global_cleanup(void);

/* --- Client management --- */
wu_client_t *wu_client_new(const char* api_key);
wu_client_t *wu_client_new_from_file(const char* file_name);
void wu_client_free(wu_client_t *client);

/* --- RAW API --- */
char *wu_fetch_current_conditions(wu_client_t *client, const char *location);

#endif //CWUNDERGROUNDAPI_WUNDERGROUND_H