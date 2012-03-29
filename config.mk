TARGET = game-c
SRCS := $(wildcard *.c)

# includes and libs
INCS = `pkg-config --cflags sdl SDL_image` \
       -I../Chipmunk-Physics/include/chipmunk/
LIBS = `pkg-config --libs sdl SDL_image gl glu` -lphysfs \
       -L../Chipmunk-Physics/src/ -lchipmunk
# OSX:
#LIBS = `pkg-config --libs sdl SDL_image` -lphysfs -framework OpenGL \
#       -L../Chipmunk-Physics/src/ -lchipmunk

# flags
CPPFLAGS = #-D
#CFLAGS   = -std=c99 -pedantic -Os -Wall ${INCS} ${CPPFLAGS} -MMD
#LDFLAGS  = -s ${LIBS}
CFLAGS   = -std=c99 -pedantic -O0 -g -pg -Wall ${INCS} ${CPPFLAGS} -MMD
LDFLAGS  = ${LIBS}

# compiler and linker
CC = cc
