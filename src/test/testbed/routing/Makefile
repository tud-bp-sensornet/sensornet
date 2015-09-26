CONTIKI = ../../../../lib/contiki
SENSORNET = ../../..

# default parameters
MAX_EDGES = 120
DISCOVERY_INTERVAL_MIN = 15
DISCOVERY_INTERVAL_MAX = 25
TTL = 1800

# load piloty1 positions
include $(SENSORNET)/test/testbed/piloty1.defines

__NEIGHBOR_DISCOVERY_DEBUG__ = 1
__ROUTING_DEBUG__ = 1

include $(SENSORNET)/Makefile.include
include $(CONTIKI)/Makefile.include
