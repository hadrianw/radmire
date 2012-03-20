#include <SDL/SDL_video.h>
#include <SDL/SDL_image.h>
#undef main
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void usage()
{
	fputs("usage: atlas -s <size> [-vcu] [-f <format><bpp>] -o target source ...\n"
	      "       atlas -s <width>x<height> [-vcu] [-f <format><bpp>] -o target source ...", stderr);
	exit(EXIT_FAILURE);
}

unsigned int width = 0;
unsigned int height = 0;
bool ignorefail = false;
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
			ignorefail = true;
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
	printf("%d %s\n", ninput, targetname);
	if(input[0])
		puts(input[0]);

        SDL_Surface **imgs = calloc(ninput, sizeof(SDL_Surface*));
        SDL_Surface *img = NULL;
        for(int i = 0; i < ninput; i++) {
                img = IMG_Load(input[i]);
                if(img)
                        imgs[i - 1] = img;
                else {
                        ninput = i;
			puts("dupa zimna");
                        goto free;
                }
        }

        //qsort(imgs, sizeof(imgs)

	ret = 0;
free:
        for(int i = 0; i < ninput; i++) {
                SDL_FreeSurface(imgs[i]);
        }
        free(imgs);
        return ret;
}
