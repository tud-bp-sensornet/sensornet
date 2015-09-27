CONTIKI = ../../../../lib/contiki
SENSORNET = ../../..

# test parameters
DISCOVERY_INTERVAL_MIN = 2
DISCOVERY_INTERVAL_MAX = 4
TTL = 1800

# load piloty1 positions
include $(SENSORNET)/test/testbed/piloty1.defines

__GRAPH_DEBUG__ = 1
__NEIGHBOR_DISCOVERY_DEBUG__ = 1

include $(SENSORNET)/Makefile.include
include $(CONTIKI)/Makefile.include
