#include "rr_types.h"

#include <SDL/SDL.h>

struct RRTex {
        unsigned int handle;
        float texcoords[4];
};

/*
struct RRTexMap {
	struct RRArray table;
}
*/

SDL_Surface *rr_loadimg(const char *path);
SDL_Surface *rr_formatimg(SDL_Surface *src);
unsigned int rr_maketex(SDL_Surface *surface);
unsigned int rr_loadtex(const char *path);

//int rr_addatlas(struct RRTexMap *map, const char *spec, const char *image);
//struct RRTex *rr_gettex(struct RRTexMap *map, const char *name);
