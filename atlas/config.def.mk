# includes and libs
INCS = `pkg-config --cflags sdl libpng`
LIBS = `pkg-config --libs-only-L sdl` \
       `pkg-config --libs-only-l sdl` \
       `pkg-config --libs libpng`

#### Win
INCS = -I../../SDL/include/
LIBS = -lmingw32 -L../../SDL/lib/ -lSDL -lpng -lz

# flags
CPPFLAGS = -D_SDL_main_h
#CFLAGS   = -std=c99 -pedantic -Os -Wall ${INCS} ${CPPFLAGS}
#LDFLAGS  = -s ${LIBS}
CFLAGS   = -std=c99 -pedantic -O0 -g -Wall ${INCS} ${CPPFLAGS}
LDFLAGS  = ${LIBS}

# compiler and linker
CC = cc
