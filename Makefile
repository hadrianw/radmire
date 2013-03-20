include config.mk

all: game-c

include contrib/SDL_image.mk

SRC = main.c \
      ${SDLIMAGESRC} \
      ${PLATFORM}
OBJ = ${SRC:.c=.o}

game-c: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ $+ ${LDFLAGS}

${OBJ}: config.mk
main.o: ${SDLIMAGEDEP}

.c.o:
	@echo CC -c $<
	@${CC} -o $@ -c $< ${CFLAGS}

clean:
	@echo cleaning
	@rm -f ${TARGET} ${OBJ}

.PHONY: all clean
