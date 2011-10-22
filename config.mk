TARGET = game-c
SRCS := $(wildcard *.c)
EXCLUDE =

# includes and libs
INCS = `pkg-config --cflags sdl`
LIBS = `pkg-config --libs sdl gl glu` -lphysfs

# flags
CPPFLAGS = #-D
CFLAGS   = -std=c99 -pedantic -Os -Wall ${INCS} ${CPPFLAGS} -MMD
LDFLAGS  = -s ${LIBS}

# compiler and linker
CC = cc
