CONTIKI = ../../../../lib/contiki

TTL = 30

PROJECT_SOURCEFILES+=../../../pbroadcast.c
CFLAGS += -I../../../

CFLAGS += -D__BROADCAST_DEBUG__=1
CFLAGS += -DTTL=$(TTL)

TESTS=$(wildcard ??-*.csc)
all: $(TESTS)

# build and call wrapper
run-unittest: $(TESTS)
	sh simexec.sh

CONTIKI_WITH_RIME = 1
include $(CONTIKI)/Makefile.include