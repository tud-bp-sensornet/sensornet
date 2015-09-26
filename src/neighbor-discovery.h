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
 * \def NO_PURGE
 * \brief Set to 1 if you don't want edges and nodes to be purged once their TTL runs out.
*/
#ifndef NO_PURGE
#define NO_PURGE 0
#endif

/**
 * \def NEIGHBOR_DISCOVERY_CHANNEL
 * \brief Rime channel neighbor discovery operates on.
*/
#ifndef NEIGHBOR_DISCOVERY_CHANNEL
#define NEIGHBOR_DISCOVERY_CHANNEL 10000
#endif

/**
 * \brief Define name of the process so it can be started by other processes just by including this file.
*/
PROCESS_NAME(neighbor_discovery_process);