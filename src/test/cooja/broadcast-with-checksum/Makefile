CONTIKI = ../../../../lib/contiki

PROJECT_SOURCEFILES+=../../../pbroadcast.c
CFLAGS += -I../../../

CFLAGS += -D__BROADCAST_DEBUG__=1

CONTIKI-PROJECT = broadcast.firmware
all: $(CONTIKI-PROJECT)

# build and call wrapper
run-unittest: $(CONTIKI-PROJECT)
	sh simexec.sh

CONTIKI_WITH_RIME = 1
include $(CONTIKI)/Makefile.include
