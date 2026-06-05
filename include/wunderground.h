#pragma once

#ifndef WUNDERGROUND_H
#define WUNDERGROUND_H

#include <stdbool.h>

#define WU_MISSING_INT INT_MIN

typedef enum { WU_XML } WUFetchType;

typedef enum { METRIC, IMPERIAL, HYBRID } WUUnitType;

typedef struct {
  WUFetchType fetchType;
  WUUnitType unitType;
} WUConfig;

typedef struct {
  const char *stationId;
  const char *apiKey;
  WUConfig config;
} WUInstance;

typedef struct {
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

typedef struct {
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

#endif // WUNDERGROUND_H
