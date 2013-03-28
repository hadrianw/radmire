#include "SDL_image.h"
#include "IMG_savepng.h"
#include <SDL/SDL_video.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

Uint32
getpx(SDL_Surface *surf, int x, int y) {
	int bpp = surf->format->BytesPerPixel;

	Uint8 *p = (Uint8 *)surf->pixels + y * surf->pitch + x * bpp;

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
putpx(SDL_Surface *surf, int x, int y, Uint32 px) {
	int bpp = surf->format->BytesPerPixel;

	Uint8 *p = (Uint8 *)surf->pixels + y * surf->pitch + x * bpp;

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

static SDL_Surface *dst;
static char *dstfname;
static SDL_Surface *src;
static char *srcfname;

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
	
        dst = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h,
	                           src->format->BitsPerPixel,
	                           src->format->Rmask,
	                           src->format->Gmask,
	                           src->format->Bmask,
	                           src->format->Amask);
	if(!dst) {
		fputs("outline: couldn't create dst surface\n", stderr);
		goto out_src;
	}

	for(int x = 0; x < dst->w; x++) {
		for(int y = 0; y < dst->h; y++) {
			putpx(dst, x, y,
				(getpx(src, x+1, y) & src->format->Amask) ||
				(getpx(src, x, y+1) & src->format->Amask) ||
				(getpx(src, x-1, y) & src->format->Amask) ||
				(getpx(src, x, y+1) & src->format->Amask)
				? 0xFFFFFF : src->format->Amask);
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

