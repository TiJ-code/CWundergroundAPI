#ifndef CWUNDERGROUNDAPI_WUNDERGROUND_H
#define CWUNDERGROUNDAPI_WUNDERGROUND_H

int wu_global_init(void);
void wu_global_cleanup(void);

typedef struct {
    const char* api_key;
    const char* base_url;
} wu_client_t;

wu_client_t *wu_client_new(const char* api_key);
void wu_client_free(wu_client_t *client);

char *wu_get_current_conditions(wu_client_t* client, const char* location);

#endif //CWUNDERGROUNDAPI_WUNDERGROUND_H