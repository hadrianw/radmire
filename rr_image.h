#include <SDL/SDL.h>

extern struct RRArray rr_map;

extern SDL_Surface *rr_loadimg(const char *path);
extern SDL_Surface *rr_formatimg(SDL_Surface *src);
extern struct RRTex *rr_loadrrtex(const char *path);
extern unsigned int rr_loadtex(const char *path);
extern unsigned int rr_maketex(SDL_Surface *surface);
extern SDL_Surface *rr_pow2img(SDL_Surface *src);

extern int rr_addatlas(struct RRArray *map, const char *spc, const char *img);
extern struct RRTex *rr_gettex(struct RRArray *map, const char *name);
extern struct RRTex *rr_findtex(struct RRArray *map, const char *name);
extern struct RRTex *rr_findntex(struct RRArray *map, size_t nel, const char *name);
extern void rr_freemaptex(struct RRArray *map);
