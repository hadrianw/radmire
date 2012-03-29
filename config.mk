TARGET = game-c
SRCS = main.c rr_array.c rr.c rrgl.c rr_image.c rr_types.c \
       utf8offset.c utils.c \
       physfsrwops.c IMG.c IMG_png.c

# includes and libs
INCS = `pkg-config --cflags sdl libpng` \
       -I../Chipmunk-Physics/include/chipmunk/
LIBS = `pkg-config --libs sdl gl glu libpng` -lphysfs \
       -L../Chipmunk-Physics/src/ -lchipmunk
# OSX:
#LIBS = `pkg-config --libs sdl libpng` -lphysfs -framework OpenGL \
#       -L../Chipmunk-Physics/src/ -lchipmunk

# flags
CPPFLAGS = #-D
#CFLAGS   = -std=c99 -pedantic -Os -Wall ${INCS} ${CPPFLAGS} -MMD
#LDFLAGS  = -s ${LIBS}
CFLAGS   = -std=c99 -pedantic -O0 -g -pg -Wall ${INCS} ${CPPFLAGS} -MMD
LDFLAGS  = ${LIBS}

# compiler and linker
CC = cc
