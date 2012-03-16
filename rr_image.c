#include "rr_image.h"

#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include "physfsrwops.h"
#include "rr.h"
#include "utils.h"

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
        struct RRimg image;
        struct ImgNode *parent;
        struct ImgNode *children[2];
} ImgNode;

float Border = 1.0f;

ImgNode *imgnode_insert(ImgNode *node, SDL_Surface *surface)
{
	//if we're not a leaf then insert
	if(node->children[0] && node->children[1]) {
		ImgNode *next = NULL;
                for(int i = 0; i < LENGTH(node->children) && !next; i++)
                        next = imgnode_insert(node->children[i], surface);
                return next;
	}

        // if there's already a image here, return
        if(node->image.surface)
                return NULL;

        // if we're too small, return
        if( surface->w + Border > node->image.srcrect.w || surface->h + Border > node->image.srcrect.h ) return NULL;

        // if we're just right, accept
        if( surface->w + Border == node->image.srcrect.w && surface->h + Border == node->image.srcrect.h ) {
                node->image.surface = surface;
                node->image.srcrect.w -= Border;
                node->image.srcrect.h -= Border;
                return node;
        }

        // otherwise, gotta split this node and create some kids
        for(int i = 0; i < LENGTH(node->children); i++) {
                node->children[i] = calloc(1, sizeof(ImgNode));
                node->children[i]->parent = node;
        }

        SDL_Rect dst = node->image.srcrect;
        SDL_Rect rest = node->image.srcrect;
        if( dst.w - surface->w > dst.h - surface->h ) {
                dst.w = surface->w + Border;
                rest.x += dst.w;
                rest.w -= dst.w;
        } else {
                dst.h = surface->h + Border;
                rest.y += dst.h;
                rest.h -= dst.h;
        }
        node->children[0]->image.srcrect = dst;
        node->children[1]->image.srcrect = rest;

        // insert into first child we created
        return imgnode_insert(node->children[0], surface);
}

void imgnode_free(ImgNode *node)
{
        if(!node)
                return;

        for(int i = 0; i < LENGTH(node->children); i++)
                imgnode_free(node->children[i]);

        if(node->parent)
                free(node);
}

// Insert source surfaces to destination, exit on first failing
struct RRimg *rrimg_atlas(SDL_Surface **srcs, unsigned int nsrcs, SDL_Surface *dst)
{
        unsigned int i = 0;
        struct RRimg *dsts = malloc(nsrcs * sizeof(struct RRimg));
        ImgNode root = { .image = {NULL, {0, 0, dst->w, dst->h}} };
        ImgNode *current = NULL;

        for(; i < nsrcs; i++) {
                current = imgnode_insert(&root, srcs[i]);
                if(current) {
                        dsts[i] = current->image;
                        dst->refcount++;
                } else
                        break;
        }
        imgnode_free(&root);
        return dsts;
}

