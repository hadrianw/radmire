# includes and libs
INCS = `pkg-config --cflags sdl SDL_image libpng`
LIBS = `pkg-config --libs-only-L sdl SDL_image` \
       `pkg-config --libs-only-l sdl SDL_image` \
       `pkg-config --libs libpng`

# flags
CPPFLAGS = -D_SDL_main_h
#CFLAGS   = -std=c99 -pedantic -Os -Wall ${INCS} ${CPPFLAGS} -MMD
#LDFLAGS  = -s ${LIBS}
CFLAGS   = -std=c99 -pedantic -O0 -g -pg -Wall ${INCS} ${CPPFLAGS} -MMD
LDFLAGS  = -pg ${LIBS}

# compiler and linker
CC = cc
