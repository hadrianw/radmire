SDLIMAGESRC = ../contrib/IMG.c ../contrib/IMG_png.c
SDLIMAGEOBJ = ${SDLIMAGESRC:.c=.o}
SDLIMAGEDEP = ../contrib/SDL_image.h

${SDLIMAGEOBJ}: ${SDLIMAGEDEP}
