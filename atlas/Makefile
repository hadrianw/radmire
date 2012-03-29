include config.mk

SRC = atlas.c ../contrib/IMG_savepng.c \
      ../contrib/IMG.c ../contrib/IMG_png.c
OBJ = ${SRC:.c=.o}

all: atlas

.c.o:
	@echo CC -c $<
	@$(CC) -o $@ -c $< $(CFLAGS)

${OBJ}: config.mk

atlas: ${OBJ}
	@echo CC -o $@
	@$(CC) -o $@ $+ $(LDFLAGS)

clean:
	@echo cleaning
	@rm -f atlas *.o

.PHONY: all clean
