# *nix
GLLIBS = -lGLU -lGL
PLATFORMLIBS = -lm
PLATFORM = ../stub/screen.c

# Win
#GLLIBS = -lopengl32 -lglu32
#PLATFORMFLAGS = -DNO_STDIO_REDIRECT
#PLATFORMLIBS = -static -lmingw32 -lgdi32 -lwinmm
#PLATFORM = ../win/SDL_win32_main.c ../win/screen.c

# OSX:
#GLLIBS = -framework OpenGL
#PLATFORM = ../stub/screen.c

# includes and libs
INCS = -I../contrib/
LIBS = -lSDL ${PLATFORMLIBS} ${GLLIBS} -lpng -lz

# flags
CPPFLAGS = ${PLATFORMFLAGS}
#CFLAGS   = -std=c99 -pedantic -Os -Wall ${INCS} ${CPPFLAGS}
#LDFLAGS  = -s ${LIBS}
CFLAGS   = -std=c99 -pedantic -O0 -g -Wall ${INCS} ${CPPFLAGS}
LDFLAGS  = ${LIBS}

# compiler and linker
CC = cc
