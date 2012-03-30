include config.mk

all: game-c

RADMIRESRC = rr.c rr_array.c rr_image.c rr_types.c rrgl.c utf8offset.c utils.c
RADMIREOBJ = ${RADMIRESRC:.c=.o}
RADMIREDEP = radmire.h rr.h rr_array.h rr_image.h rr_math.h rr_types.h \
             rrgl.h rrphysics.h utils.h

include contrib/SDL_image.mk
contrib/physfsrwops.o: contrib/physfsrwops.h

SRC = main.c ${RADMIRESRC} \
      ${SDLIMAGESRC} contrib/physfsrwops.c \
      ${PLATFORM}
OBJ = ${SRC:.c=.o}

game-c: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ $+ ${LDFLAGS}

${OBJ}: config.mk
${RADMIREOBJ}: rr_math.h rr_types.h utils.h
main.o: ${RADMIREDEP}
rr.o: rr.h rrgl.h
rr_image.o: rr.h rr_array.h rr_image.h rrgl.h \
            ${SDLIMAGEDEP} contrib/physfsrwops.h
rrgl.o: rrgl.h

.c.o:
	@echo CC -c $<
	@${CC} -o $@ -c $< ${CFLAGS}

clean:
	@echo cleaning
	@rm -f ${TARGET} ${OBJ}

.PHONY: all clean
