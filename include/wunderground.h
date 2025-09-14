#ifndef CWUNDERGROUNDAPI_WUNDERGROUND_H
#define CWUNDERGROUNDAPI_WUNDERGROUND_H

int wu_global_init(void);
void wu_global_cleanup(void);

typedef struct {
    const char* api_key;
    const char* base_url;
} wu_client_t;

wu_client_t* wu_client_new(const char* api_key);
void wu_client_delete(wu_client_t* client);

#endif //CWUNDERGROUNDAPI_WUNDERGROUND_H