include config.mk

SRC = main.c rr_array.c rr.c rrgl.c rr_image.c rr_types.c \
      utf8offset.c utils.c \
      contrib/IMG.c contrib/IMG_png.c contrib/physfsrwops.c
OBJ = ${SRC:.c=.o}

all: game-c

game-c: ${OBJ}
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
