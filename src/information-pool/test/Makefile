CONTIKI = ../../../lib/contiki
CONTIKI_WITH_RIME = 1

TARGET=native

APPS = unit-test

#include all needed C files here
PROJECT_SOURCEFILES+=../serialize.c
PROJECT_SOURCEFILES+=../graph.c
PROJECT_SOURCEFILES+=../memory.c
CFLAGS += -I../


all: serialize.test graph.test

run-unittests: all
	../../../scripts/run-unittest.sh ./*.test.native

myclean: clean
	-rm *.test.native


include $(CONTIKI)/Makefile.include
