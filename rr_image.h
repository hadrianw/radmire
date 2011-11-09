#include "rr_types.h"

#include <SDL/SDL.h>

#define RRTEX_NCOORDS 8

RRfloat coords[RRTEX_NCOORDS];

SDL_Surface *rrimg_load(const char *path);
SDL_Surface *rrimg_display_format(SDL_Surface *src);
unsigned int rrtex_load(SDL_Surface *surface);

unsigned int rrimg_atlas(SDL_Surface *srcs, unsigned int nsrcs, SDL_Surface *dst);

