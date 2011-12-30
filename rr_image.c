#include "rr_image.h"

#include <GL/gl.h>
#include <SDL/SDL_image.h>
#include "physfsrwops.h"
#include "rr.h"

SDL_Surface *rrimg_load(const char *path)
{
        SDL_RWops *rw;

        rw = PHYSFSRWOPS_openRead(path);
        if(!rw)
                return NULL;
        return IMG_Load_RW(rw, 1);
}

SDL_Surface *rrimg_display_format(SDL_Surface *src)
{
        return SDL_ConvertSurface(src, &rr_format, SDL_SWSURFACE);
}

unsigned int rrtex_load(SDL_Surface *surface)
{
        GLenum type;
        unsigned int handle;

        switch(surface->format->BitsPerPixel) {
        case 32:
                type = GL_UNSIGNED_INT_8_8_8_8;
                break;
        case 16:
                type = GL_UNSIGNED_SHORT_4_4_4_4;
                break;
        default:
                return 0;
        }
        
        glGenTextures(1, &handle);   
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel,
                        surface->w, surface->h, 0, GL_RGBA, type,
                        surface->pixels);

	/*gluBuild2DMipmaps*/

        return handle;
}

unsigned int rrteximg_load(const char *path)
{
        unsigned int handle = 0;
        SDL_Surface *orig = NULL;
        SDL_Surface *conv = NULL;

        orig = rrimg_load(path);
        if(!orig)
                goto out_orig;

        conv = rrimg_display_format(orig);
        if(!conv)
                goto out_conv;

        handle = rrtex_load(conv);

        SDL_FreeSurface(conv);
out_conv:
        SDL_FreeSurface(orig);
out_orig:
        return handle;
}

typedef struct ImgNode {
        SDL_Rect clip;
        struct ImgNode *parent;
        struct ImgNode *children[2];
} ImgNode;

// Insert source surfaces to destination, exit on first failing
// Source surfaces are modified!
unsigned int rrimg_atlas(SDL_Surface *srcs, unsigned int nsrcs, SDL_Surface *dst)
{
        unsigned int i = 0;
        ImgNode rootnode;

        for(; i < nsrcs; i++) {
                /*if(imgnode_insert(rootnode, srcs[i])) {
                        srcs[i]
                } else
                        break;*/
        }
        return 0;
}

