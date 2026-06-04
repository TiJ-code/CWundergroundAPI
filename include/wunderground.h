#pragma once

#ifndef WUNDERGROUND_H
#define WUNDERGROUND_H

#define WU_MISSING_INT INT_MIN

typedef enum { WU_XML } WUFetchType;

typedef enum { METRIC, IMPERIAL, HYBRID } WUUnitType;

typedef struct {
  WUFetchType fetchType;
  const char *stationId;
  const char *apiKey;
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

bool wu_init(char *stationId, char *apiKey, WUInstance *out);

bool wu_free(WUInstance *out);

#endif // WUNDERGROUND_H
