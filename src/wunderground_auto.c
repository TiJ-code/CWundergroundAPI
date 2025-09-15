#include <wunderground_auto.h>
#include <json-c/json.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#define sleep(seconds) Sleep((seconds) * 1000)
#else
#include <pthread.h>
#include <unistd.h>
#endif

static void parse_current_conditions(const char *json, wu_callbacks_t *callbacks) {
    if (!json || !callbacks) return;

    json_object *root = json_tokener_parse(json);
    if (!root) return;

    json_object *observations;
    if (json_object_object_get_ex(root, "current_observations", &observations)) {
        json_object *value;

        if (json_object_object_get_ex(observations, "temp_c", &value) &&
            callbacks->on_temperature_change) {
            double celciusValue = json_object_get_double(value);
            json_object *value_fahrenheit;
            if (json_object_object_get_ex(observations, "temp_f", &value_fahrenheit)) {
                double fahrenheitValue = json_object_get_double(value_fahrenheit);
                callbacks->on_temperature_change(celciusValue, fahrenheitValue, callbacks->user_data);
            }
        }

        if (json_object_object_get_ex(observations, "weather", &value) &&
            callbacks->on_condition_change) {
            callbacks->on_condition_change(json_object_get_string(value), callbacks->user_data);
        }

        if (json_object_object_get_ex(observations, "wind_kph", &value) &&
            callbacks->on_wind_change) {
            double speed_kpg = json_object_get_double(value);
            json_object *direction;
            if (json_object_object_get_ex(observations, "wind_degrees", &direction)) {
                double direction_deg = json_object_get_double(direction);
                callbacks->on_wind_change(speed_kpg, direction_deg, callbacks->user_data);
            }
        }

        if (json_object_object_get_ex(observations, "pressure_mb", &value) &&
            callbacks->on_pressure_change) {
            callbacks->on_pressure_change(json_object_get_double(value), callbacks->user_data);
        }

        if (json_object_object_get_ex(observations, "relative_humidity", &value) &&
            callbacks->on_humidity_change) {
            int percent = atoi(json_object_get_string(value));
            callbacks->on_humidity_change(percent, callbacks->user_data);
        }
    }

    json_object_put(root);
}

int wu_get_current_conditions(wu_client_t *client, wu_callbacks_t *callbacks, const char *location) {
    char *json = wu_fetch_current_conditions(client, location);
    if (!json) return -1;

    parse_current_conditions(json, callbacks);
    free(json);

    return 0;
}

struct timed_fetch_data_t {
    wu_client_t *client;
    wu_callbacks_t *callbacks;
    const char *location;
    int interval;
    volatile int running;
#ifdef _WIN32
    unsigned thread_id;
#else
    pthread_t thread_id;
#endif
};

static void *timed_fetch_thread(void *arg) {
    timed_fetch_data_t *data = arg;
    while (data->running) {
        wu_get_current_conditions(data->client, data->callbacks, data->location);
        sleep(data->interval);
    }

    free((char *)data->location);
    free(data);
    return 0;
}

timed_fetch_data_t *wu_setup_timed_callback(wu_client_t *client, wu_callbacks_t *callbacks,
                            const char *location, int interval_seconds) {
    timed_fetch_data_t *data = malloc(sizeof(timed_fetch_data_t));
    if (!data) return NULL;

    data->client = client;
    data->callbacks = callbacks;
    data->interval = interval_seconds;
    data->location = strdup(location);
    data->running = 1;

    if (!data->location) {
        free(data);
        return NULL;
    }

#ifdef _WIN32
#else
    if (pthread_create(&data->thread_id, NULL, timed_fetch_thread, data) != 0) {
        free((char *)data->location);
        free(data);
        return NULL;
    }
    pthread_detach(data->thread_id); // run thread independently
#endif

    return data;
}

void wu_stop_timed_callback(timed_fetch_data_t *data) {
    if (!data) return;
    data->running = 0;
}