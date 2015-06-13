CONTIKI = ../../lib/contiki

APPS = unit-test

PROJECT_SOURCEFILES+=serialize.c
PROJECT_SOURCEFILES+=memory.c
PROJECT_SOURCEFILES+=graph.c
PROJECT_SOURCEFILES+=bulk_broadcast.c
PROJECT_SOURCEFILES+=buffer.c

all: main send

CONTIKI_WITH_RIME = 1
include $(CONTIKI)/Makefile.include
