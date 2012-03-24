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
	struct RRvec2 pos;
	struct RRvec2 siz;
	nread = fscanf(specfile, "%lf %lf %lf %lf ",
	               &pos.x, &pos.y, &siz.x, &siz.y);
	if(nread != 4
           || pos.x < 0 || pos.x >= 1 || pos.y < 0 || pos.y >= 1
           || siz.x <= 0 || siz.x > 1 || siz.y <= 0 || siz.y > 1)
		goto free;
	tex->texcoords[0] = rr_vec2(pos.x, pos.y + siz.y);
	tex->texcoords[1] = rr_vec2_plus(pos, siz);
	tex->texcoords[2] = rr_vec2(pos.x + siz.x, pos.y);
	tex->texcoords[3] = pos;

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
        int ntex;
        for(ntex = 0; (tex = specline(map, specfile)); ) {
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
                        ntex++;
		}
        }
	fclose(specfile);

        if(!ntex) {
                glDeleteTextures(1, &atlas);
                return 0;
        }

	qsort(map->ptr, map->nmemb, map->size,
	      (int(*)(const void*, const void*))texcmp);

	return ntex;
}

struct RRTex *rr_gettex(struct RRArray *map, const char *name)
{
	if(!map || !name)
		return NULL;
	struct RRTex * tex = rr_findtex(map, name);
        if(tex)
                return tex;

        tex = malloc(sizeof(tex[0]));
        tex->handle = rr_loadtex(name);
        memcpy(tex->texcoords, rr_texcoords_identity, sizeof(tex->texcoords));
        tex->name = malloc((strlen(name) + 1) * sizeof(name[0]));
        strcpy(tex->name, name);
        rrarray_push(map, &tex);
	qsort(map->ptr, map->nmemb, map->size,
	      (int(*)(const void*, const void*))texcmp);
        return tex;
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

void rr_freemaptex(struct RRArray *map)
{
        struct RRTex **tex = map->ptr;
        for(size_t i = 0; i < map->nmemb; i++) {
                glDeleteTextures(1, &tex[i]->handle);
                freetex(tex[i]);
        }
        free(map->ptr);
        map->ptr = 0;
        map->nmemb = 0;
        map->nalloc = 0;
}
