TARGET=native

# include contiki unit test module
APPS = unit-test

# test parameters
K = 3
NODE_POSITION_ARRAY = {50,0},{90,0},{130,0},{130,40},{130,-40},{170,-40}
NODE_POSITION_ARRAY_LENGTH = 6
TTL = 180

PROJECT_SOURCEFILES+=clock.c #necessary because native has no proper clock implementation

# build all files ending with .test
run-unittests: $(patsubst %.test.c,%.test,$(wildcard *.test.c))
	for t in $^; do \
		../../scripts/unittest-wrapper ./$${t}.native; \
	done

all: run-unittests

CONTIKI = ../../lib/contiki
SENSORNET = ..

include $(SENSORNET)/Makefile.include
include $(CONTIKI)/Makefile.include