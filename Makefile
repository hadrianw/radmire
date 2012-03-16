include config.mk

all: $(TARGET)

$(TARGET): $(SRCS:.c=.o)
	@echo CC -o $@
	@$(CC) -o $@ $+ $(LDFLAGS)

include $(SRCS:.c=.d)

.c.o: config.mk
	@echo CC -c $<
	@$(CC) -c $< $(CFLAGS)

clean:
	@echo cleaning
	@rm -f $(TARGET) *.o *.d

.PHONY: all clean
