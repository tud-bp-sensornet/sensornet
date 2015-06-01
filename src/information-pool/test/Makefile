CONTIKI = ../../../lib/contiki
CONTIKI_WITH_RIME = 1

TARGET=native

APPS = unit-test

#include all needed C files here
PROJECT_SOURCEFILES+=../serialize.c
CFLAGS += -I../


all: serialize.test

run-unittests: all
	../../../scripts/run-unittest.sh ./*.test.native

myclean: clean
	-rm *.test.native


include $(CONTIKI)/Makefile.include
