TARGET=native
APPS = unit-test

# default parameters
K = 3
MAX_NODES = 20
MAX_EDGES = 100
NODE_POSITION_ARRAY = {50,0},{90,0},{130,0},{130,40},{130,-40},{170,-40}
NODE_POSITION_ARRAY_LENGTH = 6

CFLAGS += -Wall

CFLAGS += -DK=$(K)
CFLAGS += -DMAX_NODES=$(MAX_NODES)
CFLAGS += -DMAX_EDGES=$(MAX_EDGES)
CFLAGS += -DNODE_POSITION_ARRAY=$(NODE_POSITION_ARRAY)
CFLAGS += -DNODE_POSITION_ARRAY_LENGTH=$(NODE_POSITION_ARRAY_LENGTH)

# include all needed C files here
#PROJECT_SOURCEFILES+=
PROJECT_SOURCEFILES+=../graph.c
PROJECT_SOURCEFILES+=../graph-operations.c
PROJECT_SOURCEFILES+=../serialize.c
PROJECT_SOURCEFILES+=../routing.c
PROJECT_SOURCEFILES+=../positions.c
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
