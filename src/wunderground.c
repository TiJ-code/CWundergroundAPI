#include "wunderground.h"

#include "curl/curl.h"

#include <curl/easy.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct wu_instance {
  const char *stationId;
  const char *apiKey;
  WUConfig config;

  CURL *curl;
  char *responseBuffer;
};

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

  instance->responseBuffer = NULL;

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

  free((void *)instance->stationId);
  free((void *)instance->apiKey);
  free(instance);

  return true;
}

bool wu_isCurlInitialized(WUInstance *instance) {
  return instance && instance->curl != NULL;
}
