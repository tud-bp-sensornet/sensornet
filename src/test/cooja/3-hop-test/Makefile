CONTIKI = ../../../../lib/contiki

# default parameters
K = 3
MAX_NODES = 20
MAX_EDGES = 100

CFLAGS += -Wall

CFLAGS += -DK=$(K)
CFLAGS += -DMAX_NODES=$(MAX_NODES)
CFLAGS += -DMAX_EDGES=$(MAX_EDGES)

PROJECT_SOURCEFILES+=../../../graph.c
PROJECT_SOURCEFILES+=../../../graph-operations.c
PROJECT_SOURCEFILES+=../../../serialize.c
PROJECT_SOURCEFILES+=../../../pbroadcast.c
PROJECT_SOURCEFILES+=../../../malloc.c
CFLAGS += -I../../../

CFLAGS += -D__K_HOP_DEBUG__=1

TESTS=$(wildcard ??-*.csc)
all: $(TESTS)

# build and call wrapper
run-unittest: $(TESTS)
	sh simexec.sh

CONTIKI_WITH_RIME = 1
include $(CONTIKI)/Makefile.include
