/**
 * \file neighbor-discovery.h
 * Header file for the neighbor discovery process.
 * \author tud-bp-sensornet
 */

#include "contiki.h"

/**
 * \def DISCOVERY_INTERVAL_MIN
 * \brief Minimum interval between neighbor discovery broadcasts.
*/
#ifndef DISCOVERY_INTERVAL_MIN
#define DISCOVERY_INTERVAL_MIN 60
#endif

/**
 * \def DISCOVERY_INTERVAL_MAX
 * \brief Maximum interval between neighbor discovery broadcasts.
*/
#ifndef DISCOVERY_INTERVAL_MAX
#define DISCOVERY_INTERVAL_MAX 120
#endif

/**
 * \brief Define name of the process so it can be started by other processes just by including this file.
*/
PROCESS_NAME(neighbor_discovery_process);