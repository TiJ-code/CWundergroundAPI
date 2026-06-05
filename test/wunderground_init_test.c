#include "test.h"
#include "wunderground.h"

int main(void) {
  char *stationId = "ABCDEFGH";
  char *apiKey = "abcdefghijklmnopqrstuvwxyz012345";

  WUInstance *instance =
      wu_init(stationId, apiKey,
              (WUConfig){.fetchType = WU_XML, .unitType = WU_METRIC});

  ASSERT(instance != NULL, "wu_init returned NULL");
  ASSERT(wu_isCurlInitialized(instance) == true, "curl not initialized");
  ASSERT(wu_free(instance) == true, "wu_free failed");

  return 0;
}
