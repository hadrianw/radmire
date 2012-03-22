#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include "physfsrwops.h"
#include "rr.h"
#include "utils.h"

SDL_Surface *rr_loadimg(const char *path)
{
        SDL_RWops *rw;

        rw = PHYSFSRWOPS_openRead(path);
        if(!rw)
                return NULL;
        return IMG_Load_RW(rw, 1);
}

SDL_Surface *rr_formatimg(SDL_Surface *src)
{
        return SDL_ConvertSurface(src, &rr_format, SDL_SWSURFACE);
}

unsigned int rr_maketex(SDL_Surface *surface)
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

unsigned int rr_loadtex(const char *path)
{
        unsigned int handle = 0;
        SDL_Surface *orig = NULL;
        SDL_Surface *conv = NULL;

        orig = rr_loadimg(path);
        if(!orig)
                goto out_orig;

        conv = rr_formatimg(orig);
        if(!conv)
                goto out_conv;

        handle = rr_maketex(conv);

        SDL_FreeSurface(conv);
out_conv:
        SDL_FreeSurface(orig);
out_orig:
        return handle;
}

int rr_addatlas(struct RRArray *map, const char *spec, const char *image)
{
	if(!map || !spec || !image)
		return 0;
	
	FILE *specfile = fopen(spec, "rb");
	if(!spec)
		return 0;

	unsigned int atlas = rr_loadtex(image);
	rrgl_bind_texture(atlas);

	struct RRTex *tex;

	char buff[BUFSIZ];
	tex = calloc(1, sizeof(tex[0]));
	int nread = 0;
	nread = fscanf(specfile,
	               "%f %f %f %f ",
	               &tex->texcoords[0],
        	       &tex->texcoords[1],
        	       &tex->texcoords[2],
        	       &tex->texcoords[3]);
	if(nread != 4)
		goto free_tex;
	fgets(buff, LENGTH(buff), specfile);	
	tex->name = malloc((strlen(buff) + 1) * sizeof(tex->name[0]));
	strcpy(tex->name, buff);

	fclose(specfile);
	return 0;
free_tex:
	free(tex);
	fclose(specfile);
	return 0;
}

struct RRTex *rr_gettex(struct RRArray *map, const char *name)
{
	if(!map || !name)
		return NULL;

	return NULL;
}
