#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>

int main(int argc, char **argv)
{
        int ret = 0;
        int nimgs = argc - 1;
        SDL_Surface **imgs = calloc(nimgs, sizeof(SDL_Surface*));
        SDL_Surface *img = NULL;
        for(int i = 1; i < argc; i++) {
                img = IMG_Load(argv[i]);
                if(img)
                        imgs[i - 1] = img;
                else {
                        nimgs = i;
                        ret = -1;
                        goto free;
                }
        }

        qsort(imgs, sizeof(imgs)

free:
        for(int i = 0; i < nimgs; i++) {
                SDL_FreeSurface(imgs[i]);
        }
        free(imgs);
        return ret;
}
