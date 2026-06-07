#include "wunderground.h"

#include "curl/curl.h"

#include <curl/easy.h>
#include <curl/typecheck-gcc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WU_API_URL "https://https://api.weather.com/v2/pws/observations/current"

struct wu_instance {
  const char *stationId;
  const char *apiKey;
  WUConfig config;

  CURL *curl;
  char *responseBuffer;
  size_t responseSize;
};

static inline char _wu_getUnitType(WUUnitType unitType) {
  switch (unitType) {
  case WU_METRIC:
    return 'm';
  case WU_IMPERIAL:
    return 'e';
  case WU_HYBRID:
    return 'h';
  default:
    return 'm';
  }
}

static inline char *_wu_getFetchType(WUFetchType fetchType) {
  switch (fetchType) {
  case WU_XML:
    return "xml";
  default:
    return "xml";
  }
}

static char *_wu_buildURL(WUInstance *instance) {
  size_t bufSize = 512;

  char *url = (char *)malloc(bufSize);

  if (!url)
    return NULL;

  snprintf(url, bufSize, "%s?stationId=%s&format=%s&units=%c&apiKey=%s",
           WU_API_URL, instance->stationId,
           _wu_getFetchType(instance->config.fetchType),
           _wu_getUnitType(instance->config.unitType), instance->apiKey);

  return url;
}

static size_t _wu_curl_writeCallback(void *contents, size_t size, size_t nmemb,
                                     void *userp) {
  size_t total = size * nmemb;

  WUInstance *instance = (WUInstance *)userp;

  char *newBuffer =
      realloc(instance->responseBuffer, instance->responseSize + total + 1);

  if (!newBuffer)
    return 0;

  instance->responseBuffer = newBuffer;

  memcpy(instance->responseBuffer + instance->responseSize, contents, total);

  instance->responseSize += total;

  instance->responseBuffer[instance->responseSize] = 0;

  return total;
}

WUInstance *wu_init(char *stationId, char *apiKey, WUConfig config) {
  if (!stationId || strlen(stationId) != WU_STATION_ID_LENGTH) {
    perror("stationId is either null or not 8 chars long!");
    return NULL;
  }

  if (!apiKey || strlen(apiKey) != WU_API_KEY_LENGTH) {
    perror("apiKey is either null or not 32 chars long!");
    return NULL;
  }

  WUInstance *instance = (WUInstance *)malloc(sizeof(*instance));

  if (!instance) {
    perror("WUInstance creation failed!");
    return NULL;
  }

  instance->stationId = strdup(stationId);
  instance->apiKey = strdup(apiKey);
  instance->config = config;

  instance->curl = curl_easy_init();
  if (!instance->curl) {
    free((void *)instance->stationId);
    free((void *)instance->apiKey);
    free(instance);
    return NULL;
  }

  curl_easy_setopt(instance->curl, CURLOPT_WRITEFUNCTION,
                   _wu_curl_writeCallback);
  curl_easy_setopt(instance->curl, CURLOPT_WRITEDATA, instance);

  instance->responseBuffer = (char *)malloc(512);

  return instance;
}

bool wu_free(WUInstance *instance) {
  if (!instance) {
    perror("WUInstance is null!");
    return false;
  }

  if (instance->curl) {
    curl_easy_cleanup(instance->curl);
  }

  free(instance->responseBuffer);
  free((void *)instance->stationId);
  free((void *)instance->apiKey);
  free(instance);

  return true;
}

bool wu_isCurlInitialized(WUInstance *instance) {
  return instance && instance->curl != NULL;
}

bool wu_fetch(WUInstance *instance, WUObservation *out) {
  if (!instance || !out)
    return false;

  char *url = _wu_buildURL(instance);

  if (!url)
    return false;

  curl_easy_setopt(instance->curl, CURLOPT_URL, url);

  CURLcode result = curl_easy_perform(instance->curl);

  free(url);

  if (result != CURLE_OK)
    return false;

  printf("%s\n", instance->responseBuffer);

  return true;
}

bool wu_fetch_verbose(WUInstance *instance, WUObservationVerbose *out) {
  return false;
}
