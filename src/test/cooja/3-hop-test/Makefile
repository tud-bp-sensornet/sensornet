CONTIKI = ../../../../lib/contiki

# default parameters
K = 3
MAX_NODES = 20
MAX_EDGES = 100
DISCOVERY_INTERVAL_MIN = 2
DISCOVERY_INTERVAL_MAX = 4

CFLAGS += -Wall

CFLAGS += -DK=$(K)
CFLAGS += -DMAX_NODES=$(MAX_NODES)
CFLAGS += -DMAX_EDGES=$(MAX_EDGES)
CFLAGS += -DDISCOVERY_INTERVAL_MIN=$(DISCOVERY_INTERVAL_MIN)
CFLAGS += -DDISCOVERY_INTERVAL_MAX=$(DISCOVERY_INTERVAL_MAX)


PROJECT_SOURCEFILES+=../../../graph.c
PROJECT_SOURCEFILES+=../../../graph-operations.c
PROJECT_SOURCEFILES+=../../../serialize.c
PROJECT_SOURCEFILES+=../../../pbroadcast.c
PROJECT_SOURCEFILES+=../../../neighbor-discovery.c
CFLAGS += -I../../../

CFLAGS += -D__K_HOP_DEBUG__=1

TESTS=$(wildcard ??-*.csc)
all: $(TESTS)

# build and call wrapper
run-unittest: $(TESTS)
	sh simexec.sh

CONTIKI_WITH_RIME = 1
include $(CONTIKI)/Makefile.include
