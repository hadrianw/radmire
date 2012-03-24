#include <SDL/SDL.h>

extern struct RRArray rr_map;

SDL_Surface *rr_loadimg(const char *path);
SDL_Surface *rr_formatimg(SDL_Surface *src);
unsigned int rr_maketex(SDL_Surface *surface);
unsigned int rr_loadtex(const char *path);

extern int rr_addatlas(struct RRArray *map, const char *spc, const char *img);
extern struct RRTex *rr_gettex(struct RRArray *map, const char *name);
extern struct RRTex *rr_findtex(struct RRArray *map, const char *name);
extern struct RRTex *rr_findntex(struct RRArray *map, size_t nel, const char *name);
