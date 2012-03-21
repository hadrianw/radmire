#include <SDL/SDL_video.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ispow2(X) (((X) & ((X) - 1)) == 0)
#define LENGTH(X) (sizeof(X) / sizeof (X)[0])
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

struct Img {
	SDL_Surface *surf;
	const char *name;
};

struct ImgNode {
	SDL_Surface *image;
	SDL_Rect rect;
        struct ImgNode *parent;
        struct ImgNode *children[2];
};

static int imgcomp(const void* b, const void* a);
static struct ImgNode *imgnode_insert(struct ImgNode *node, SDL_Surface *surface);
static void usage();

static bool failstop = true;
static bool sortinput = true;
static bool verbose = false;
static int border = 1;
static unsigned int width = 0;
static unsigned int height = 0;
static const char *targetname  = NULL;
static char **input = NULL;
static int ninput = 0;

int main(int argc, char **argv)
{
        int ret = EXIT_FAILURE;

	for(int i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "-c"))
			failstop = false;
		else if(!strcmp(argv[i], "-u"))
			sortinput = false;
		else if(!strcmp(argv[i], "-v"))
			verbose = true;
		else if(i+1 == argc)
			usage();
		else if(!strcmp(argv[i], "-b")) {
			border = atoi(argv[++i]);
		} else if(!strcmp(argv[i], "-s")) {
			int ss = sscanf(argv[++i], "%ux%u", &width, &height);
			if(ss == 1)
				height = width;
			else if(ss != 2)
				return ret;
		} else if(!strcmp(argv[i], "-o")) {
			targetname = argv[++i];
			i++;
			ninput = argc - i;
			input = argv + i;
			break;
		} else
			usage();
	}
	if(border <= 0 || !width || !height || !ispow2(width) || !ispow2(height)
	   || !targetname || ninput <= 0 || !input)
		usage();

        struct Img *imgs = calloc(ninput, sizeof(struct Img));
        for(int i = 0; i < ninput; i++) {
		if(verbose)
			printf("loading %s\n", input[i]);
		imgs[i].name = input[i];
                if( !(imgs[i].surf = IMG_Load(input[i])) ) {
			fprintf(stderr, "atlas: couldn't load %s\n", input[i]);
			if(failstop) {
				ninput = i;
				goto free;
			}
                }
        }

	if(sortinput) {
		qsort(imgs, ninput, sizeof(imgs[0]), imgcomp);
		if(verbose) {
			puts("sort order:");
			for(int i = 0; i < ninput; i++)
				puts(imgs[i].name);

		}
	}

        unsigned int i = 0;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	Uint32 rmsk = 0xff000000;
	Uint32 gmsk = 0x00ff0000;
	Uint32 bmsk = 0x0000ff00;
	Uint32 amsk = 0x000000ff;
#else
	Uint32 rmsk = 0x000000ff;
	Uint32 gmsk = 0x0000ff00;
	Uint32 bmsk = 0x00ff0000;
	Uint32 amsk = 0xff000000;
#endif
	SDL_Surface *target = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32,
	                                           rmsk, gmsk, bmsk, amsk);
	SDL_FillRect(target, NULL, 0xFFFFFFFF);
        struct ImgNode root = { .rect = {0, 0, target->w, target->h} };
        struct ImgNode *node = NULL;

	float invwidth = 1.0f / width;
	float invheight = 1.0f / height;
        for(; i < ninput; i++) {
		if(imgs[i].surf) {
			node = imgnode_insert(&root, imgs[i].surf);
			if(node) {
				int e = SDL_BlitSurface(node->image, NULL, target, &node->rect);
				printf("err %d\n", e);
				printf("%s %f %f %f %f\n", imgs[i].name,
				       node->rect.x * invwidth, node->rect.y * invheight,
				       node->rect.w * invwidth, node->rect.h * invheight);
			} else if(failstop) {
				fprintf(stderr, "atlas: couldn't fit %s\n", input[i]);
				goto free;
			}
		} else if(sortinput)
			break;
        }
	printf("err: %d\n", SDL_SaveBMP(target, targetname));

        //imgnode_free(&root);

	ret = 0;
free:
        for(int i = 0; i < ninput; i++) {
                SDL_FreeSurface(imgs[i].surf);
        }
        free(imgs);
        return ret;
}

int imgcomp(const void* b, const void* a)
{
	const SDL_Surface *A = ((struct Img*)a)->surf;
	const SDL_Surface *B = ((struct Img*)b)->surf;
	if(!A || !B)
		return (A ? 1 : 0) - (B ? 1 : 0);
	else
		return MAX(A->w, A->h) - MAX(B->w, B->h);
}

struct ImgNode *imgnode_insert(struct ImgNode *node, SDL_Surface *surface)
{
	//if we're not a leaf then insert
	if(node->children[0] && node->children[1]) {
		struct ImgNode *next = NULL;
                for(int i = 0; i < LENGTH(node->children) && !next; i++)
                        next = imgnode_insert(node->children[i], surface);
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
        for(int i = 0; i < LENGTH(node->children); i++) {
                node->children[i] = calloc(1, sizeof(struct ImgNode));
                node->children[i]->parent = node;
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
        node->children[0]->rect = dst;
        node->children[1]->rect = rest;

        return imgnode_insert(node->children[0], surface);
}

void usage()
{
	fputs("usage: atlas -s <size> [-vcu] [-f <format><bpp>] -o target source ...\n"
	      "       atlas -s <width>x<height> [-vcu] [-f <format><bpp>] -o target source ...\n", stderr);
	exit(EXIT_FAILURE);
}

