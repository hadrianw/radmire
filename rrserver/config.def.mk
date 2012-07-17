# includes and libs
INCS = -I../../enet-1.3.3/include/
LIBS = -L../../enet-1.3.3/.libs/ -lenet
PLATFORM = 

# flags
CPPFLAGS = -D_POSIX_SOURCE
#CFLAGS   = -std=c99 -pedantic -Os -Wall ${INCS} ${CPPFLAGS}
#LDFLAGS  = -s ${LIBS}
CFLAGS   = -std=c99 -pedantic -O0 -g -Wall ${INCS} ${CPPFLAGS}
LDFLAGS  = ${LIBS}

# compiler and linker
CC = cc
