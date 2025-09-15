#ifndef CWUNDERGROUNDAPI_WUNDERGROUND_AUTO_H
#define CWUNDERGROUNDAPI_WUNDERGROUND_AUTO_H

#include "wunderground.h"

/* --- Callback types --- */
typedef void (*wu_temperature_callback)(double celcius, double fahrenheit, void *user);
typedef void (*wu_conditions_callback)(const char *description, void *user);
typedef void (*wu_wind_callback)(double speed_kph, double direction_deg, void *user);
typedef void (*wu_pressure_callback)(double hpa, void *user);
typedef void (*wu_humidity_callback)(int percent, void *user);

/* --- Callback registry --- */
typedef struct {
    wu_temperature_callback on_temperature_change;
    wu_conditions_callback on_condition_change;
    wu_wind_callback on_wind_change;
    wu_pressure_callback on_pressure_change;
    wu_humidity_callback on_humidity_change;
    void *user_data;
} wu_callbacks_t;

/* --- Auto API --- */
int wu_get_current_conditions(wu_client_t *client,
                              wu_callbacks_t *callbacks,
                              const char *location);

typedef struct timed_fetch_data_t timed_fetch_data_t;

timed_fetch_data_t *wu_setup_timed_callback(wu_client_t *client,
                            wu_callbacks_t *callbacks,
                            const char *location,
                            int interval_seconds);

void wu_stop_timed_callback(timed_fetch_data_t *handle);

#endif //CWUNDERGROUNDAPI_WUNDERGROUND_AUTO_H