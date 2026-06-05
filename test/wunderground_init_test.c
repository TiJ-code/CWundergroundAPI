#include "wunderground.h"

int main(void) {
  char *stationId = "";
  char *apiKey = "";

  WUInstance *instance = wu_init(
      stationId, apiKey, (WUConfig){.fetchType = WU_XML, .unitType = METRIC});

  if (!instance)
    return 1;

  if (!wu_free(instance))
    return 1;

  return 0;
}
