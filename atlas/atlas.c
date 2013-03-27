#include "SDL_image.h"
#include "IMG_savepng.h"
#include <SDL/SDL_video.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define ispow2(X) (((X) & ((X) - 1)) == 0)
#define LENGTH(X) (sizeof(X) / sizeof (X)[0])
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define COLOR_SHIFT(X) (X)
#else
#define COLOR_SHIFT(X) (3 - (X))
#endif

struct Img {
	SDL_Surface *image;
	const char *name;
};

struct ImgNode {
	SDL_Surface *image;
	SDL_Rect rect;
        struct ImgNode *parent;
        struct ImgNode *child[2];
};

static void cleanup(int status);
static SDL_Surface *createsurface(int width, int height, int bpp);
static void genatlas();
static int imgcomp(const void* b, const void* a);
static void imgnode_free(struct ImgNode *node);
static struct ImgNode *imgnode_insert(struct ImgNode *node, SDL_Surface *surface);
static void loadsources();
static void openspec();
static void saveimage();
static void usage();

static const char specext[] = ".atlas";
static const char imageext[] = ".png";

static bool failstop = true;
static bool filltarget = false;
static bool sortinput = true;
static bool outext = true;
static bool verbose = false;
static int border = 1;
static unsigned int width = 0;
static unsigned int height = 0;
static const char *targetname = NULL;
static char **sourcename = NULL;
static int nsources = 0;

static struct Img *source = NULL;
static char *filename = NULL;
static size_t filenamesize = 0;
static struct ImgNode root = { 0 };
static FILE *spec = NULL;
static SDL_Surface *target = NULL;

int main(int argc, char **argv)
{
	for(int i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "-c"))
			failstop = false;
		else if(!strcmp(argv[i], "-f"))
                        filltarget = true;
		else if(!strcmp(argv[i], "-u"))
			sortinput = false;
		else if(!strcmp(argv[i], "-v"))
			verbose = true;
		else if(!strcmp(argv[i], "-x"))
                        outext = false;
		else if(i+1 == argc)
			usage();
		else if(!strcmp(argv[i], "-b"))
			border = atoi(argv[++i]);
		else if(!strcmp(argv[i], "-s")) {
			int ss = sscanf(argv[++i], "%ux%u", &width, &height);
			if(ss == 1)
				height = width;
			else if(ss != 2)
				usage();
		} else if(!strcmp(argv[i], "-o")) {
			targetname = argv[++i];
			i++;
			nsources = argc - i;
			sourcename = argv + i;
			break;
		} else
			usage();
	}
	if(border <= 0
	   || !width || !height || !ispow2(width) || !ispow2(height)
	   || !targetname || nsources <= 0 || !sourcename)
		usage();

	loadsources();

	if(sortinput) {
		qsort(source, nsources, sizeof(source[0]), imgcomp);
		if(verbose) {
			puts("sort order:");
			for(int i = 0; i < nsources; i++)
				puts(source[i].name);

		}
	}

	genatlas();
	saveimage();

	cleanup(0);
        return 0;
}

void cleanup(int status)
{
        imgnode_free(&root);
	SDL_FreeSurface(target);
	free(filename);
        for(int i = 0; i < nsources; i++)
                SDL_FreeSurface(source[i].image);
        free(source);

	if(status)
		exit(status);
}

SDL_Surface *createsurface(int width, int height, int bpp)
{
	SDL_Surface *surf;
        Uint32 rmsk = ((1 << bpp / 4) - 1) << bpp / 4 * COLOR_SHIFT(3);
        Uint32 gmsk = ((1 << bpp / 4) - 1) << bpp / 4 * COLOR_SHIFT(2);
        Uint32 bmsk = ((1 << bpp / 4) - 1) << bpp / 4 * COLOR_SHIFT(1);
        Uint32 amsk = ((1 << bpp / 4) - 1) << bpp / 4 * COLOR_SHIFT(0);

        surf = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, bpp,
	                            rmsk, gmsk, bmsk, amsk);
        if(surf && filltarget)
                SDL_FillRect(surf, NULL, rmsk | amsk);
        
        return surf;
}

void genatlas()
{
	openspec();

	target = createsurface(width, height, 32);
	if(!target) {
		fputs("atlas: couldn't create target surface\n", stderr);
		cleanup(EXIT_FAILURE);
	}
        root.rect.w = target->w;
        root.rect.h = target->h;

	double invwidth = 1.0 / width;
	double invheight = 1.0 / height;
        struct ImgNode *node = NULL;
        int len;
        for(int i = 0; i < nsources; i++) {
		if(!source[i].image) {
			if(sortinput) {
				nsources = i;
				break;
			} else
				continue;
		}
		if(verbose)
			printf("fitting %s\n", source[i].name);
		node = imgnode_insert(&root, source[i].image);
		if(!node) {
			fprintf(stderr, "atlas: couldn't fit %s\n", source[i].name);
			if(failstop)
				cleanup(EXIT_FAILURE);
			continue;
		}
		if(SDL_BlitSurface(node->image, NULL, target, &node->rect)) {
			fprintf(stderr, "atlas: couldn't blit %s\n", source[i].name);
			cleanup(EXIT_FAILURE);
		}

                char *dot;
                if(!outext && (dot = strrchr(source[i].name, '.')))
                        len = dot - source[i].name;
                else
                        len = strlen(source[i].name);

		fprintf(spec, "%.8f %.8f %.8f %.8f %.*s\n",
		        node->rect.x * invwidth, node->rect.y * invheight,
		        node->rect.w * invwidth, node->rect.h * invheight,
		        len, source[i].name);
        }
	fclose(spec);
}

int imgcomp(const void* b, const void* a)
{
	const SDL_Surface *A = ((struct Img*)a)->image;
	const SDL_Surface *B = ((struct Img*)b)->image;
	if(!A || !B)
		return (A ? 1 : 0) - (B ? 1 : 0);
	else
		return MAX(A->w, A->h) - MAX(B->w, B->h);
}

void imgnode_free(struct ImgNode *node)
{
        if(!node)
                return;

        for(int i = 0; i < LENGTH(node->child); i++)
                imgnode_free(node->child[i]);

        if(node->parent)
                free(node);
}

struct ImgNode *imgnode_insert(struct ImgNode *node, SDL_Surface *surface)
{
	//if we're not a leaf then insert
	if(node->child[0] && node->child[1]) {
		struct ImgNode *next = NULL;
                for(int i = 0; i < LENGTH(node->child) && !next; i++)
                        next = imgnode_insert(node->child[i], surface);
                return next;
	}

        if(node->image
	   || surface->w + border > node->rect.w
	   || surface->h + border > node->rect.h)
		return NULL;

        if(surface->w + border == node->rect.w &&
	   surface->h + border == node->rect.h) {
                node->image = surface;
                node->rect.w -= border;
                node->rect.h -= border;
                return node;
        }

        // otherwise, gotta split this node and create some kids
        for(int i = 0; i < LENGTH(node->child); i++) {
                node->child[i] = calloc(1, sizeof(struct ImgNode));
                node->child[i]->parent = node;
        }

        SDL_Rect dst = node->rect;
        SDL_Rect rest = node->rect;
        if( dst.w - surface->w > dst.h - surface->h ) {
                dst.w = surface->w + border;
                rest.x += dst.w;
                rest.w -= dst.w;
        } else {
                dst.h = surface->h + border;
                rest.y += dst.h;
                rest.h -= dst.h;
        }
        node->child[0]->rect = dst;
        node->child[1]->rect = rest;

        return imgnode_insert(node->child[0], surface);
}

void loadsources()
{
        source = calloc(nsources, sizeof(source[0]));
        for(int i = 0; i < nsources; i++) {
		if(verbose)
			printf("loading %s\n", sourcename[i]);
		source[i].image = IMG_Load(sourcename[i]);
		source[i].name = sourcename[i];
                if(source[i].image)
			SDL_SetAlpha(source[i].image, 0, SDL_ALPHA_OPAQUE);
		else {
			fprintf(stderr, "atlas: couldn't load %s\n", sourcename[i]);
			if(failstop) {
				nsources = i;
				cleanup(EXIT_FAILURE);
			}
                }
        }
}

void openspec()
{
	filenamesize = strlen(targetname) + MAX(LENGTH(specext), LENGTH(imageext));
        filename = malloc(filenamesize * sizeof(filename[0]));
	strcpy(filename, targetname);
	strcat(filename, specext);
	if(verbose)
		printf("saving spec %s\n", filename);
        spec = fopen(filename, "w+b");
	if(!spec) {
		fprintf(stderr, "atlas: couldn't open %s:", filename);
		cleanup(EXIT_FAILURE);
	}
}

void saveimage()
{
	strcpy(filename, targetname);
	strcat(filename, imageext);
	if(verbose)
		printf("saving image %s\n", filename);
	if(IMG_SavePNG(filename, target, 9)) {
		fprintf(stderr, "atlas: couldn't save image %s\n", filename);
		cleanup(EXIT_FAILURE);
	}
}

void usage()
{
	fputs("usage: atlas -s <size> [-c] [-f] [-u] [-v]"
                          " -o target source ...\n"
	      "       atlas -s <width>x<height> [-c] [-f] [-u] [-v]"
                          " -o target source ...\n",
	      stderr);
	cleanup(EXIT_FAILURE);
}

