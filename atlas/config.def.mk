# *nix
PLATFORMLIBS = -lm

# Win
#PLATFORMLIBS = -static -lmingw32
#PLATFORM = win/SDL_win32_main.c

# includes and libs
INCS = 
LIBS = -lSDL ${PLATFORMLIBS} -lpng -lz

# flags
CPPFLAGS = -D_SDL_main_h
#CFLAGS   = -std=c99 -pedantic -Os -Wall ${INCS} ${CPPFLAGS}
#LDFLAGS  = -s ${LIBS}
CFLAGS   = -std=c99 -pedantic -O0 -g -Wall ${INCS} ${CPPFLAGS}
LDFLAGS  = ${LIBS}

# compiler and linker
CC = cc
