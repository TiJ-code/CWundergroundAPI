#include "wunderground.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

WUInstance *wu_init(char *stationId, char *apiKey, WUConfig config) {
  WUInstance *instance = (WUInstance *)malloc(sizeof(*instance));

  if (!instance) {
    perror("WUInstance creation failed!");
    return NULL;
  }

  instance->stationId = strdup(stationId);
  instance->apiKey = strdup(apiKey);
  memcpy(&config, &(instance->config), sizeof(instance->config));

  return instance;
}

bool wu_free(WUInstance *instance) {
  if (!instance) {
    perror("WUInstance is null!");
    return false;
  }

  free((void *)instance->stationId);
  free((void *)instance->apiKey);
  free(instance);

  instance->stationId = NULL;
  instance->apiKey = NULL;
  instance->config = (WUConfig){0};

  return true;
}
