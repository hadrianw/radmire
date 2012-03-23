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

struct RRArray rr_map = {
	.size = sizeof(struct RRTex*)
};

static int texcmp(const struct RRTex **a, const struct RRTex **b)
{
	// FIXME: is this possible:
	if(!a || !b)
		return (a ? 1 : 0) - (b ? 1 : 0);

	return strcmp((*a)->name, (*b)->name);
}

static int strtexcmp(const char *a, const struct RRTex **b)
{
	// FIXME: is this possible:
	if(!a || !b)
		return (a ? 1 : 0) - (b ? 1 : 0);

	return strcmp(a, (*b)->name);
}

static void freetex(struct RRTex *tex)
{
	if(!tex)
		return;
	free(tex->name);
	free(tex);
}

struct RRTex *specline(struct RRArray *map, FILE *specfile)
{
	static char buff[BUFSIZ];
	struct RRTex *tex;
	tex = calloc(1, sizeof(tex[0]));

	int nread = 0;
	nread = fscanf(specfile,
	               "%f %f %f %f ",
	               &tex->texcoords[0],
        	       &tex->texcoords[1],
        	       &tex->texcoords[2],
        	       &tex->texcoords[3]);
	if(nread != 4
           || tex->texcoords[0] < 0 || tex->texcoords[0] >= 1
           || tex->texcoords[1] < 0 || tex->texcoords[1] >= 1
           || tex->texcoords[2] <= 0 || tex->texcoords[2] > 1
           || tex->texcoords[3] <= 0 || tex->texcoords[3] > 1)
		goto free;
        tex->texcoords[2] += tex->texcoords[0];
        tex->texcoords[3] += tex->texcoords[1];

	fgets(buff, LENGTH(buff), specfile);	
	size_t bufflen = strlen(buff);
	if(buff[bufflen-1] == '\n')
		buff[bufflen-1] = '\0';
	else
		goto free;
	tex->name = malloc((bufflen + 1) * sizeof(tex->name[0]));
	strcpy(tex->name, buff);

        return tex;

free:
        freetex(tex);
        return NULL;
}

int rr_addatlas(struct RRArray *map, const char *spec, const char *image)
{
	if(!map || !spec || !image)
		return 0;
	
	FILE *specfile = fopen(spec, "rb");
	if(!spec)
		return 0;

	unsigned int atlas = rr_loadtex(image);

	size_t nprev = map->nmemb;
	struct RRTex *tex;
	struct RRTex *dup;
	printf("map: nmemb %lu nalloc %lu size %lu\n",
	       map->nmemb, map->nalloc, map->size);
        while( (tex = specline(map, specfile)) ) {
		tex->handle = atlas;
		dup = rr_findntex(map, nprev, tex->name);
		// Ignore this line on duplicate, or we could:
		// memset(dup, tex,
		//        sizeof(struct RRTex) - sizeof(tex->name));
		// freetex(tex);
		// tex = dup;
		if(dup) {
			printf("duplicate found: %s\n", tex->name);
			freetex(tex);
		} else {
			printf("inserting: %s\n", tex->name);
			rrarray_push(map, &tex);
		}
        }

	qsort(map->ptr, map->nmemb, map->size,
	      (int(*)(const void*, const void*))texcmp);

	fclose(specfile);
	return 0;
/*
free_tex:
	free(tex);
	fclose(specfile);
	return 0;
	*/
}

struct RRTex *rr_findtex(struct RRArray *map, const char *name)
{
	if(!map || !name)
		return NULL;
	return rr_findntex(map, map->nmemb, name);
}

struct RRTex *rr_findntex(struct RRArray *map, size_t nel, const char *name)
{
	if(!map || !name)
		return NULL;

	struct RRTex **match;
	match = bsearch(name, map->ptr, nel, map->size,
	                (int(*)(const void*, const void*))strtexcmp);

	return match ? *match : NULL;
}
