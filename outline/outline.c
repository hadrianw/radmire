#include "SDL_image.h"
#include "IMG_savepng.h"
#include <SDL/SDL_video.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define LENGTH(X)    (sizeof(X) / sizeof(X)[0])
#define ALPHA 0xFF000000
#define SKIP 0xFFFFFF

typedef struct {
	char x, y;
} Vec2;

static Uint32 getpx(SDL_Surface *surf, int x, int y);
static void line(SDL_Surface *surf, int x0, int y0, int x1, int y1, Uint32 px);
static void outline(int bx, int by);
static void putpx(SDL_Surface *surf, int x, int y, Uint32 px);

static Vec2 pxdir[] = {
	{  0, -1 }, /* N  */
	{  1, -1 }, /* NE */
	{  1,  0 }, /*  E */
	{  1,  1 }, /* SE */
	{  0,  1 }, /* S  */
	{ -1,  1 }, /* SW */
	{ -1,  0 }, /*  W */
	{ -1, -1 }  /* NW */
};
static SDL_Surface *dst;
static char *dstfname;
static SDL_Surface *src;
static char *srcfname;

Uint32
getpx(SDL_Surface *surf, int x, int y) {
	int bpp = surf->format->BytesPerPixel;

	if(x < 0 || y < 0 || x >= surf->w || y >= surf->h)
		return 0;

	Uint8 *p = (Uint8*)surf->pixels + y * surf->pitch + x * bpp;

	switch(bpp) {
	case 1:
		return *p;
		break;
	case 2:
		return *(Uint16*)p;
		break;
	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;
	case 4:
		return *(Uint32*)p;
		break;
	default:
		return 0;
	}
}
 
void
line(SDL_Surface *surf, int x0, int y0, int x1, int y1, Uint32 px) {
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1; 
	int err = (dx > dy ? dx : -dy)/2, e2;

	for(;;){
		putpx(surf, x0, y0, px);
		if(x0 == x1 && y0 == y1)
			break;
		e2 = err;
		if(e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		if(e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}
}

void
outline(int bx, int by) {
	int x = bx, y = by;
	int nx, ny;
	int next;
	char d, off = 0;

	do {
		for(d = 0; d < LENGTH(pxdir); d++) {
			next = (off + d) % LENGTH(pxdir);
			nx = x + pxdir[next].x;
			ny = y + pxdir[next].y;
			if(getpx(src, nx, ny) & ALPHA) {
				x = nx;
				y = ny;
				putpx(dst, x, y, SKIP);
				off = (next - 2) % LENGTH(pxdir);
				break;
			}
		}
	} while(x != bx || y != by);
}

void
putpx(SDL_Surface *surf, int x, int y, Uint32 px) {
	int bpp = surf->format->BytesPerPixel;

	if(x < 0 || y < 0 || x >= surf->w || y >= surf->h)
		return;

	Uint8 *p = (Uint8*)surf->pixels + y * surf->pitch + x * bpp;

	switch(bpp) {
	case 1:
		*p = px;
		break;
	case 2:
		*(Uint16*)p = px;
		break;
	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (px >> 16) & 0xff;
			p[1] = (px >> 8) & 0xff;
			p[2] = px & 0xff;
		} else {
			p[0] = px & 0xff;
			p[1] = (px >> 8) & 0xff;
			p[2] = (px >> 16) & 0xff;
		}
		break;
	case 4:
		*(Uint32*)p = px;
		break;
	}
}

int
main(int argc, char **argv) {
	if(argc < 3) {
		fputs("usage: outline src dst\n", stderr);
		return EXIT_FAILURE;
	}

	dstfname = argv[2];
	srcfname = argv[1];

	src = IMG_Load(srcfname);
	if(!src) {
		fprintf(stderr, "outline: couldn't load image %s\n", srcfname);
		return EXIT_FAILURE;
	}
	
        dst = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, 24,
	                           0x0000FF, 0x00FF00, 0xFF0000, 0);

	if(!dst) {
		fputs("outline: couldn't create dst surface\n", stderr);
		goto out_src;
	}

	int x, y;
	bool prev, curr;
	
	for(y = 0; y < dst->h; y++) {
		prev = false;
		for(x = 0; x < dst->w; x++) {
			curr = getpx(src, x, y) & ALPHA;
			if(!prev && curr && getpx(dst, x, y) == 0)
				outline(x, y);
			prev = curr;
		}
	}

	if(IMG_SavePNG(dstfname, dst, 9)) {
		fprintf(stderr, "outline: couldn't save image %s\n", dstfname);
		goto out_dst;
	}

out_dst:
	SDL_FreeSurface(dst);
out_src:
	SDL_FreeSurface(src);
        return src && dst ? 0 : EXIT_FAILURE;
}

