# includes and libs
INCS = `pkg-config --cflags sdl libpng` \
       -I../Chipmunk-Physics/include/chipmunk/
LIBS = `pkg-config --libs sdl gl glu libpng` -lphysfs \
       -L../Chipmunk-Physics/src/ -lchipmunk
PLATFORM = posix/rrsleep.c

#### Win
#INCS = -I../SDL/include/ -DNO_STDIO_REDIRECT \
#       -I../physfs/ \
#       -I../Chipmunk-Physics/include/chipmunk/
#LIBS = -lmingw32 \
#       -L../SDL/lib/ -lSDLmain -lSDL -lopengl32 -lglu32 \
#       -lpng -L../physfs/ -lphysfs -lz \
#       -L../Chipmunk-Physics/src/ -lchipmunk
#PLATFORM = win/SDL_win32_main.c win/rrsleep.c

#### OSX:
#LIBS = `pkg-config --libs sdl libpng` -lphysfs -framework OpenGL \
#       -L../Chipmunk-Physics/src/ -lchipmunk

# flags
CPPFLAGS = #-D
#CFLAGS   = -std=c99 -pedantic -Os -Wall ${INCS} ${CPPFLAGS}
#LDFLAGS  = -s ${LIBS}
CFLAGS   = -std=c99 -pedantic -O0 -g -Wall ${INCS} ${CPPFLAGS}
LDFLAGS  = ${LIBS}

# compiler and linker
CC = cc
