#include "contiki.h"

// Minimum interval between neighbor discovery broadcasts.
#ifndef DISCOVERY_INTERVAL_MIN
#define DISCOVERY_INTERVAL_MIN 60
#endif

// Maximum interval between neighbor discovery broadcasts.
#ifndef DISCOVERY_INTERVAL_MAX
#define DISCOVERY_INTERVAL_MAX 120
#endif

// Define name of the process so it can be started by other processes just by including this file.
PROCESS_NAME(neighbor_discovery_process);