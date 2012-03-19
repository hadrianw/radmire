include config.mk

SRC = atlas.c
OBJ = ${SRC:.c=.o}

all: atlas

.c.o:
	@echo CC -c $<
	@$(CC) -c $< $(CFLAGS)

${OBJ}: config.mk

include ${SRC:.c=.d}
${SRC:.c=.d}:
	touch $@

atlas: ${OBJ}
	@echo CC -o $@
	@$(CC) -o $@ $+ $(LDFLAGS)

clean:
	@echo cleaning
	@rm -f atlas *.o

.PHONY: all clean
