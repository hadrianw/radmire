include config.mk

all: outline

include ../contrib/SDL_image.mk

SRC = outline.c ../contrib/IMG_savepng.c \
      ${SDLIMAGESRC}
OBJ = ${SRC:.c=.o}

outline: ${OBJ}
	@echo CC -o $@
	@$(CC) -o $@ $+ $(LDFLAGS)

${OBJ}: config.mk
outline.o: ${SDLIMAGEDEP} ../contrib/IMG_savepng.h
../contrib/IMG_savepng.o: ../contrib/IMG_savepng.h

.c.o:
	@echo CC -c $<
	@$(CC) -o $@ -c $< $(CFLAGS)

clean:
	@echo cleaning
	@rm -f outline *.o

.PHONY: all clean
