include config.mk

BIN = rrserver rrclient

all: ${BIN}

SRC = server.c client.c
OBJ = ${SRC:.c=.o}

rrserver: server.o
rrclient: client.o

${BIN}:
	@echo CC -o $@
	@${CC} -o $@ $+ ${LDFLAGS}

${OBJ}: config.mk

.c.o:
	@echo CC -c $<
	@${CC} -o $@ -c $< ${CFLAGS}

clean:
	@echo cleaning
	@rm -f ${TARGET} ${OBJ}

.PHONY: all clean
