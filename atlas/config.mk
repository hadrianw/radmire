# includes and libs
INCS = `pkg-config --cflags sdl SDL_image`
LIBS = `pkg-config --libs-only-L --libs-only-l sdl SDL_image`

# flags
CPPFLAGS = #-D
#CFLAGS   = -std=c99 -pedantic -Os -Wall ${INCS} ${CPPFLAGS} -MMD
#LDFLAGS  = -s ${LIBS}
CFLAGS   = -std=c99 -pedantic -O0 -g -pg -Wall ${INCS} ${CPPFLAGS} -MMD
LDFLAGS  = -pg ${LIBS}

# compiler and linker
CC = cc
