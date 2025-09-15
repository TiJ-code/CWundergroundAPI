#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "wunderground_auto.h"

void temperature_cb(double celcius, double fahrenheit, void *user) {
    printf("[Temperature]");
    fflush(stdout);
}

void conditions_cb(const char *description, void *user) {
    printf("[Conditions]");
    fflush(stdout);
}

void wind_cb(double speed_kph, double dir_deg, void *user) {
    printf("[Wind]");
    fflush(stdout);
}

void pressure_cb(double hpa, void *user) {
    printf("[Pressure]");
    fflush(stdout);
}

void humidity_cb(int percent, void *user) {
    printf("[Humidity]");
    fflush(stdout);
}

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

    wu_callbacks_t callbacks = {0};
    callbacks.on_temperature_change = temperature_cb;
    callbacks.on_condition_change = conditions_cb;
    callbacks.on_wind_change = wind_cb;
    callbacks.on_pressure_change = pressure_cb;
    callbacks.on_humidity_change = humidity_cb;

    if (wu_get_current_conditions(client, &callbacks, "DE/Berlin") != 0) {
        fprintf(stderr, "Failed to fetch weather\n");
    }

    printf("\n--- Setting up timed callbacks (10 seconds) ---\n");

    wu_timed_callback_data_t *timer = wu_setup_timed_callback(client, &callbacks, "DE/Berlin", 10);
    if (!timer) {
        fprintf(stderr, "Failed to setup timed callbacks\n");
        wu_client_free(client);
        wu_global_cleanup();
        return 1;
    }

    printf("Running timed callback for 30 seconds...\n");
#ifdef _WIN32
        Sleep(30 * 1000);
#else
        sleep(30);
#endif

    wu_stop_timed_callback(timer);
    printf("Timed callback stopped!\n");

    wu_client_free(client);
    wu_global_cleanup();

    return 0;
}