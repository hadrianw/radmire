# *nix
GLLIBS = -lgl -lglu

# Win
#GLLIBS = -lopengl32 -lglu32
#PLATFORMFLAGS = -DNO_STDIO_REDIRECT
#PLATFORMLIBS = -lmingw32 -lgdi32 -lwinmm
#PLATFORM = win/SDL_win32_main.c

# OSX:
#GLLIBS = -framework OpenGL

# includes and libs
INCS = 
LIBS = -static -lSDL ${PLATFORMLIBS} ${GLLIBS} -lpng -lz

# flags
CPPFLAGS = ${PLATFORMFLAGS}
#CFLAGS   = -std=c99 -pedantic -Os -Wall ${INCS} ${CPPFLAGS}
#LDFLAGS  = -s ${LIBS}
CFLAGS   = -std=c99 -pedantic -O0 -g -Wall ${INCS} ${CPPFLAGS}
LDFLAGS  = ${LIBS}

# compiler and linker
CC = cc
