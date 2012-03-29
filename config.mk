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
#CFLAGS   = -std=c99 -pedantic -Os -Wall ${INCS} ${CPPFLAGS}
#LDFLAGS  = -s ${LIBS}
CFLAGS   = -std=c99 -pedantic -O0 -g -pg -Wall ${INCS} ${CPPFLAGS}
LDFLAGS  = ${LIBS}

# compiler and linker
CC = cc
