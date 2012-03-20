#include <SDL/SDL_video.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ispow2(x) (((x) & ((x) - 1)) == 0)
#define MAX(x, y) ((x) > (y) ? (x) : (y))

void usage()
{
	fputs("usage: atlas -s <size> [-vcu] [-f <format><bpp>] -o target source ...\n"
	      "       atlas -s <width>x<height> [-vcu] [-f <format><bpp>] -o target source ...\n", stderr);
	exit(EXIT_FAILURE);
}

struct Img {
	SDL_Surface *surf;
	const char *name;
};

int imgcomp(const void* b, const void* a)
{
	const SDL_Surface *A = ((struct Img*)a)->surf;
	const SDL_Surface *B = ((struct Img*)b)->surf;
	if(!A || !B)
		return (A ? 1 : 0) - (B ? 1 : 0);
	else
		return MAX(A->w, A->h) - MAX(B->w, B->h);
}

unsigned int width = 0;
unsigned int height = 0;
bool failstop = true;
bool sortinput = true;
bool verbose = false;
const char *targetname  = NULL;
int ninput = 0;
char **input = NULL;

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
