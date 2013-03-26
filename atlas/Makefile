include config.mk

all: atlas

include ../contrib/SDL_image.mk

SRC = atlas.c ../contrib/IMG_savepng.c \
      ${SDLIMAGESRC}
OBJ = ${SRC:.c=.o}

atlas: ${OBJ}
	@echo CC -o $@
	@$(CC) -o $@ $+ $(LDFLAGS)

${OBJ}: config.mk
atlas.o: ${SDLIMAGEDEP} ../contrib/IMG_savepng.h
../contrib/IMG_savepng.o: ../contrib/IMG_savepng.h

.c.o:
	@echo CC -c $<
	@$(CC) -o $@ -c $< $(CFLAGS)

clean:
	@echo cleaning
	@rm -f atlas *.o

.PHONY: all clean
