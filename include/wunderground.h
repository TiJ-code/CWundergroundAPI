#pragma once

#ifndef WUNDERGROUND_H
#define WUNDERGROUND_H

#include <stdbool.h>

#define WU_MISSING_INT INT_MIN
#define WU_STATION_ID_LENGTH 8
#define WU_API_KEY_LENGTH 32

typedef enum { WU_XML } WUFetchType;

typedef enum { WU_METRIC, WU_IMPERIAL, WU_HYBRID } WUUnitType;

typedef struct wu_config {
  WUFetchType fetchType;
  WUUnitType unitType;
} WUConfig;

typedef struct wu_instance WUInstance;

typedef struct wu_observation {
  WUUnitType units;

  float solarRadition;
  int uvIndex;

  int windDirection;
  int windChill;
  int windSpeed;
  int windGust;

  int airHumidity;
  float airPressure;

  int temperature;
  int heatIndex;

  float rainRate;
  float rainTotal;
} WUObservation;

typedef struct wu_observation_verbose {
  char *stationId;
  char *observeTimeUTC;
  char *observeTimeLocal;
  char *neighborhood;
  char *country;
  float longitude;
  float latitude;
  int elevation;
  WUObservation metrics;
} WUObservationVerbose;

WUInstance *wu_init(char *stationId, char *apiKey, WUConfig config);

bool wu_free(WUInstance *out);

bool wu_isCurlInitialized(WUInstance *instance);

#endif // WUNDERGROUND_H
