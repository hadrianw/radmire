#include <SDL/SDL.h>

struct RRTex {
        unsigned int handle;
        float texcoords[4];
	char *name;
        int nrefs;
};

SDL_Surface *rr_loadimg(const char *path);
SDL_Surface *rr_formatimg(SDL_Surface *src);
unsigned int rr_maketex(SDL_Surface *surface);
unsigned int rr_loadtex(const char *path);
