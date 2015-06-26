TARGET=native
APPS = unit-test

# include all needed C files here
#PROJECT_SOURCEFILES+=

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
