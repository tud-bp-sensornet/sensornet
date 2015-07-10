TARGET=native
APPS = unit-test

# default parameters
K = 3
MAX_NODES = 20
MAX_EDGES = 100

CFLAGS += -Wall

CFLAGS += -DK=$(K)
CFLAGS += -DMAX_NODES=$(MAX_NODES)
CFLAGS += -DMAX_EDGES=$(MAX_EDGES)

# include all needed C files here
#PROJECT_SOURCEFILES+=
PROJECT_SOURCEFILES+=../graph.c
PROJECT_SOURCEFILES+=../graph-operations.c
PROJECT_SOURCEFILES+=clock.c #necessary because native has no proper clock implementation

# add ../ to include path
CFLAGS += -I../

# build all files ending with .test
run-unittests: $(patsubst %.test.c,%.test,$(wildcard *.test.c))
	for t in $^; do \
		../../scripts/unittest-wrapper ./$${t}.native; \
	done

all: run-unittests

CONTIKI = ../../lib/contiki
CONTIKI_WITH_RIME = 1

include $(CONTIKI)/Makefile.include
