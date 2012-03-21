#include <SDL/SDL_video.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ispow2(X) (((X) & ((X) - 1)) == 0)
#define LENGTH(X) (sizeof(X) / sizeof (X)[0])
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

static int imgcomp(const void* b, const void* a);
static void usage();

struct Img {
	SDL_Surface *surf;
	const char *name;
};

struct ImgNode {
	struct Img *image;
	SDL_Rect rect;
        struct ImgNode *parent;
        struct ImgNode *children[2];
};

static unsigned int width = 0;
static unsigned int height = 0;
static bool failstop = true;
static bool sortinput = true;
static bool verbose = false;
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
		else if(!strcmp(argv[i], "-s")) {
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
	if(!width || !height || !ispow2(width) || !ispow2(height)
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

void usage()
{
	fputs("usage: atlas -s <size> [-vcu] [-f <format><bpp>] -o target source ...\n"
	      "       atlas -s <width>x<height> [-vcu] [-f <format><bpp>] -o target source ...\n", stderr);
	exit(EXIT_FAILURE);
}

