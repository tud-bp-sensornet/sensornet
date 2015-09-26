CONTIKI = ../../../../lib/contiki
SENSORNET = ../../..

# test parameters
K = 3
DISCOVERY_INTERVAL_MIN = 2
DISCOVERY_INTERVAL_MAX = 4
NODE_POSITION_ARRAY = {50,0},{90,0},{130,0},{130,40},{130,-40},{170,-40}
NODE_POSITION_ARRAY_LENGTH = 6
TTL = 50

TESTS=$(wildcard ??-*.csc)
all: $(TESTS)

# build and call wrapper
run-unittest: $(TESTS)
	sh simexec.sh

include $(SENSORNET)/Makefile.include
include $(CONTIKI)/Makefile.include