include config.mk

all: ${TARGET}

${TARGET}: ${OBJ}
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
