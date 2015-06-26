CONTIKI_PROJECT = sensornet
all: $(CONTIKI_PROJECT)

# default parameters
K = 3
MAX_NODES = 20
MAX_EDGES = 100

CFLAGS += -Wall

CFLAGS += -DK=$(K)
CFLAGS += -DMAX_NODES=$(MAX_NODES)
CFLAGS += -DMAX_EDGES=$(MAX_EDGES)

CONTIKI=../lib/contiki
CONTIKI_WITH_RIME = 1

include $(CONTIKI)/Makefile.include
