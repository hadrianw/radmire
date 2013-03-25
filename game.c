#include <assert.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "contrib/SDL_image.h"

/* macros */
typedef double coord;
#define GL_COORD GL_DOUBLE
#define sqrtc sqrt
#define sinc sin
#define cosc cos
#define asinc asin
#define acosc acos
#define atan2c atan2

/*
typedef float coord;
#define GL_COORD GL_FLOAT
#define sqrtc srtf
#define sinc sinf
#define cosc cosf
#define asinc asinf
#define acosc acosf
#define atan2c atan2f
*/

#define VEC2PLUS(V1, V2)  (Vec2){(V1).x + (V2).x, (V1).y + (V2).y}
#define VEC2MINUS(V1, V2) (Vec2){(V1).x - (V2).x, (V1).y - (V2).y}
#define VEC2SQLEN(V)      ((V).x * (V).x + (V).y * (V).y)
#define VEC2LEN(V)        (sqrtc(VEC2SQLEN(V)))
#define TFORMVEC2(T, V) \
	(Vec2){ (T).pos.x + (T).col1.x * (V).x + (T).col2.x * (V).y, \
	        (T).pos.y + (T).col1.y * (V).x + (T).col2.y * (V).y }
#define TFORMRVEC2(T, V) \
	(Vec2){ (T).col1.x * (V).x + (T).col2.x * (V).y, \
	        (T).col1.y * (V).x + (T).col2.y * (V).y }
#define TFORMT(T) \
	(Tform){ { (T).col1.x, (T).col2.x}, \
	         { (T).col1.y, (T).col2.y}, \
	         (T).pos }
#define TFORMMUL(A, B) \
	(Tform){ { (A).col1.x * (B).col1.x + (A).col2.x * (B).col1.y, \
	           (A).col1.y * (B).col1.x + (A).col2.y * (B).col1.y }, \
	         { (A).col1.x * (B).col2.x + (A).col2.x * (B).col2.y, \
	           (A).col1.y * (B).col2.x + (A).col2.y * (B).col2.y }, \
	         { (A).col1.x * (B).pos.x + (A).col2.x * (B).pos.y + (A).pos.x, \
	           (A).col1.y * (B).pos.x + (A).col2.y * (B).pos.y + (A).pos.y } }

#define ISPOW2(X)         (((X) & ((X) - 1)) == 0)
#define LENGTH(X)         (sizeof(X) / sizeof(X)[0])
#define MAX(A, B)         ((A) > (B) ? (A) : (B))
#define MIN(A, B)         ((A) < (B) ? (A) : (B))

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define COLOR_SHIFT(X)    (3 - (X))
#else
#define COLOR_SHIFT(X)    (X)
#endif

#define PRESSING_BTN(btn) (input.mouse.pressed[btn] && input.mouse.changed[btn])
#define PRESSING_KEY(key) (input.keyboard.pressed[key] && input.keyboard.changed[key])

enum { BATCH_VERTS = 16384 };
enum { SDL_MAX_BUTTONS = 255 };
enum { Diagonal, Vertical, NoneBase, Horizontal }; /* aspect base */

typedef struct {
        coord x;
        coord y;
} Vec2;

typedef struct {
	int x;
	int y;
} Vec2i;

typedef struct {
        Vec2 col1;
        Vec2 col2;
        Vec2 pos;
} Tform;

typedef struct {
        void *ptr;
        size_t nalloc;
        size_t nmemb;
        size_t size;
} Array;

typedef struct {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha;
} Color;

typedef struct {
        unsigned int handle;
        Vec2 coords[4];
	char *name;
} Texture;

typedef struct {
	GLuint activetex;
	GLenum mode;
	Tform tform;
	Color color;
	Vec2 *vertices;
	Color *colors;
	Vec2 *texcoords;
	struct {
		unsigned int count;
		Vec2 vertices[BATCH_VERTS];
		Color colors[BATCH_VERTS];
		Vec2 texcoords[BATCH_VERTS];
	} array;
} Batch;

typedef struct {
	SDL_Event event;
	struct {
		bool pressed[SDLK_LAST];
		bool changed[SDLK_LAST];
	} keyboard;
	struct {
		Vec2 abs;
		Vec2 rel;
		Vec2 screenabs;
		Vec2 screenrel;
		bool moved;
		bool pressed[SDL_MAX_BUTTONS];
		bool changed[SDL_MAX_BUTTONS];
	} mouse;
} Input;

typedef struct {
        Tform t;
        Vec2 s;
} Object;

typedef struct {
	Vec2i *size;
	Vec2i winsize;
	Vec2i fullsize;
	int bpp;
	SDL_PixelFormat format;
	bool full;

	Tform tform;
	int base;
	coord top;
	coord left;
	coord bottom;
	coord right;
} Screen;

typedef struct {
	unsigned int fps;
	int ticks;
	double step;
	clock_t clockstep;
	clock_t time;
	clock_t prev;
	clock_t diff;
} Timer;

/* extern function declarations */
extern Uint32 fullscreenflag(bool fullscreen);

/* function declarations */
static size_t arrayflipremove(Array *array, size_t index);
static void arrayfree(Array *array);
static size_t arraypush(Array *array, void *src);
static size_t arrayremove(Array *array, size_t index);
static void arrayresize(Array *array, size_t nmemb);
static void batch_bind_texture(GLuint texture);
static void batch_draw_arrays(GLenum mode, GLint first, GLsizei count);
static void batch_draw_elements(GLenum mode, GLsizei count, const unsigned int *indices);
static void batch_draw_rect(const Vec2 *size, const Vec2 *align);
static void batch_flush(void);
static void batch_init(void);
static void beginscene(void);
static void beginframe(void);
static void endframe(void);
static void endscene(void);
static Texture *findntex(Array *map, size_t nel, const char *name);
static Texture *findtex(Array *map, const char *name);
static SDL_Surface *formatimg(SDL_Surface *src);
static void freetex(Texture *tex);
static void freetexmap(Array *map);
static Texture *gettex(Array *map, const char *name);
static void glinit();
static int init();
static int loadatlas(Array *map, const char *spc, const char *img);
static unsigned int loadtex(const char *path);
static Texture *loadtexstruct(const char *path);
static unsigned int maketex(SDL_Surface *surface);
static SDL_Surface *pow2img(SDL_Surface *src);
static int resize(int width, int height, bool fullscreen, int base);
static void setbasediagonal(int width, int height);
static void setbasehorizontal(int width, int height);
static void setbasenone(int width, int height);
static void setbasevertical(int width, int height);
static int setfullscreen(int base);
static int setwindowed(int base);
static void sleepc(clock_t iv);
static Texture *specline(Array *map, FILE *specfile);
static int strtexcmp(const char *a, const Texture **b);
static int texcmp(const Texture **a, const Texture **b);
static Tform tformfromvec2(const Vec2 v);
static void tformsetangle(Tform *t, coord angle);
static uint32_t topow2(uint32_t v);

/* variables */
static Batch batch = { .mode = GL_QUADS };
static const Color colorwhite = {0xFF, 0xFF, 0xFF, 0xFF};
static Input input;
static Array objects = { .size = sizeof(Object) };
static bool running = false;
static Screen screen = {
	.format = {
		.alpha = 255
	},
	.full = false
};
static void (*setbase[])(int, int) = {
	[Diagonal] = setbasediagonal,
        [Vertical] = setbasevertical,
        [NoneBase] = setbasenone,
        [Horizontal] = setbasehorizontal
};
static const Vec2 texcoordsidentity[4] = {
	{0.0f, 1.0f},
	{1.0f, 1.0f},
	{1.0f, 0.0f},
	{0.0f, 0.0f}
};
static Array texmap = { .size = sizeof(Texture*) };
static const Tform tformidentity = {
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f}
};
static Timer timer = { .fps = 60 };
static const Vec2 vec2zero;

/* function implementations */
size_t
arrayflipremove(Array *array, size_t index) {
/* removes element at index
 * moves last element to removed element
 * returns index of moved element or/and size of resized array
 */
        size_t last = array->nmemb - 1;
        if(last != index) {
                char *ptr = array->ptr;
                index *= array->size;
                last *= array->size;
                memcpy(ptr + index, ptr + last, array->size);
        }
        arrayresize(array, array->nmemb - 1);
        return array->nmemb;
}

void
arrayfree(Array *array) {
        free(array->ptr);
        array->ptr = 0;
        array->nmemb = 0;
        array->nalloc = 0;
}

size_t
arraypush(Array *array, void *src) {
        size_t last = array->nmemb * array->size;
        arrayresize(array, array->nmemb + 1);
        memcpy((char*)array->ptr + last, src, array->size);
        return last;
}

size_t
arrayremove(Array *array, size_t index) {
        char *ptr = array->ptr;
        ptr += index * array->size;
        memmove(ptr, ptr + array->size, (array->nmemb - index - 1) * array->size);
        arrayresize(array, array->nmemb - 1);
        return array->nmemb;
}

void
arrayresize(Array *array, size_t nmemb) {
        size_t nalloc = topow2(nmemb);
        if(nmemb < array->nmemb && array->nalloc > nalloc * 2)
                nalloc *= 2;
        array->ptr = realloc(array->ptr, nalloc * array->size);
        array->nalloc = nalloc;
        array->nmemb = nmemb;
}

void
batch_bind_texture(GLuint texture) {
        if(batch.activetex == texture)
                return;

	batch_flush();
        glBindTexture(GL_TEXTURE_2D, texture);
        batch.activetex = texture;
}

void
batch_draw_arrays(GLenum mode, GLint first, GLsizei count) {
        unsigned int i;
        if(count > BATCH_VERTS) {
                fprintf(stderr, "count: %d > BATCH_VERTS: %d", count, BATCH_VERTS);
                return;
        }
        if(batch.mode != mode || batch.array.count + count > BATCH_VERTS) {
                batch_flush();
                batch.mode = mode;
        }
        for(i = 0; i < count; ++i)
                batch.array.vertices[batch.array.count + i]
                        = TFORMVEC2(batch.tform, batch.vertices[first + i]);
        if(batch.colors)
                memcpy(batch.array.colors + batch.array.count,
		       batch.colors + first,
                       sizeof(*batch.colors) * count);
        else
                for(i = 0; i < count; ++i)
                        batch.array.colors[batch.array.count + i] = batch.color;
        if(batch.texcoords)
                memcpy(batch.array.texcoords + batch.array.count,
		       batch.texcoords + first,
                       sizeof(*batch.texcoords) * count);

        batch.array.count += count;
}

void
batch_draw_elements(GLenum mode, GLsizei count, const unsigned int *indices) {
        unsigned int i;
        if(count > BATCH_VERTS) {
                fprintf(stderr, "count: %d > BATCH_VERTS: %d", count, BATCH_VERTS);
                return;
        }
        if(batch.mode != mode || batch.array.count + count > BATCH_VERTS) {
                batch_flush();
                batch.mode = mode;
        }
        for(i = 0; i < count; ++i)
                batch.array.vertices[batch.array.count + i]
                        = TFORMVEC2(batch.tform, batch.vertices[indices[i]]);
        if(batch.colors)
                for(i = 0; i < count; ++i)
                        batch.array.colors[batch.array.count + i]
                                = batch.colors[indices[i]];
        else
                for(i = 0; i < count; ++i)
                        batch.array.colors[batch.array.count + i] = batch.color;
        if(batch.texcoords)
                for(i = 0; i < count; ++i)
                        batch.array.texcoords[batch.array.count + i]
                                = batch.texcoords[indices[i]];

        batch.array.count += count;
}

void
batch_draw_rect(const Vec2 *size, const Vec2 *align) {
        if(!size)
                return;

        if(!align)
                align = &(Vec2){0.5f, 0.5f};
        /* 3 ,--, 2
             | /|
             |/ |
           0 '--' 1 */
        Vec2 vs[4] = {
                {size->x *       - align->x , size->y * (align->y - 1.0f)},
                {size->x * (1.0f - align->x), size->y * (align->y - 1.0f)},
                {size->x * (1.0f - align->x), size->y *  align->y        },
                {size->x *       - align->x , size->y *  align->y        }
        };
        static const unsigned int is[2 * 3] = {0, 2, 3, 0, 1, 2};

	batch.vertices = vs;
        batch.colors = NULL;
        batch_draw_elements(GL_TRIANGLES, LENGTH(is), is);
	batch.vertices = NULL;
}

void
batch_flush() {
        glVertexPointer(2, GL_COORD, 0, batch.array.vertices);
        glTexCoordPointer(2, GL_COORD, 0, batch.array.texcoords);
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, batch.array.colors);

        if(batch.mode < GL_TRIANGLES)
                glDisable(GL_TEXTURE_2D);
        glDrawArrays(batch.mode, 0, batch.array.count);
        if(batch.mode < GL_TRIANGLES)
                glEnable(GL_TEXTURE_2D);
        batch.array.count = 0;
}

void
batch_init(void) {
        batch.color = colorwhite;
        batch.tform = tformidentity;
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
}

void
beginframe(void) {
	memset(input.keyboard.changed, 0, sizeof(input.keyboard.changed));
	memset(input.mouse.changed, 0, sizeof(input.mouse.changed));
	input.mouse.rel = vec2zero;
        while(SDL_PollEvent(&input.event)) {
                switch(input.event.type) {
                case SDL_QUIT:
                        running = false;
                        break;
                case SDL_MOUSEMOTION:
                        input.mouse.rel.x += input.event.motion.xrel;
                        input.mouse.rel.y += input.event.motion.yrel;
                        input.mouse.abs.x = input.event.motion.x;
                        input.mouse.abs.y = input.event.motion.y;
			input.mouse.screenabs = TFORMVEC2(screen.tform,
					input.mouse.abs); 
			input.mouse.screenrel = TFORMRVEC2(screen.tform,
					input.mouse.rel); 
                        input.mouse.moved = true;
                        break;
                case SDL_MOUSEBUTTONDOWN:
                        input.mouse.pressed[input.event.button.button - 1] = true;
                        input.mouse.changed[input.event.button.button - 1] = true;
                        break;
                case SDL_MOUSEBUTTONUP:
                        input.mouse.pressed[input.event.button.button - 1] = false;
                        input.mouse.changed[input.event.button.button - 1] = true;
                        break;
                case SDL_KEYDOWN:
                        input.keyboard.pressed[input.event.key.keysym.sym] = true;
                        input.keyboard.changed[input.event.key.keysym.sym] = true;
                        break;
                case SDL_KEYUP:
                        input.keyboard.pressed[input.event.key.keysym.sym] = false;
                        input.keyboard.changed[input.event.key.keysym.sym] = true;
                        break;
                case SDL_VIDEORESIZE:
                        resize(input.event.resize.w, input.event.resize.h,
                               screen.full, screen.base);
                        break;
                }
        }
}

void
beginscene(void) {
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
}

void
endframe(void) {
        timer.time = clock();
        timer.diff = timer.time - timer.prev;

        if(timer.diff < timer.clockstep) {
                timer.diff = timer.clockstep - timer.diff;
		sleepc(timer.diff);
                timer.ticks++;
                if(timer.ticks == timer.fps) {
                        printf("%f\n", timer.diff / (double)CLOCKS_PER_SEC);
                        timer.ticks = 0;
                }
        }

        timer.prev = timer.time;
}

void
endscene(void) {
        batch_flush();
        glFlush();
        SDL_GL_SwapBuffers();
}

Texture*
findntex(Array *map, size_t nel, const char *name) {
	if(!map || !name)
		return NULL;

	Texture **match;
	match = bsearch(name, map->ptr, nel, map->size,
	                (int(*)(const void*, const void*))strtexcmp);

	return match ? *match : NULL;
}

Texture*
findtex(Array *map, const char *name) {
	if(!map || !name)
		return NULL;
	return findntex(map, map->nmemb, name);
}

SDL_Surface*
formatimg(SDL_Surface *src) {
        return SDL_ConvertSurface(src, &screen.format, SDL_SWSURFACE);
}

void
freetex(Texture *tex) {
	if(!tex)
		return;
	free(tex->name);
	free(tex);
}

void
freetexmap(Array *map) {
        Texture **tex = map->ptr;
        for(size_t i = 0; i < map->nmemb; i++) {
                glDeleteTextures(1, &tex[i]->handle);
                freetex(tex[i]);
        }
        arrayfree(map);
}

Texture*
gettex(Array *map, const char *name) {
	if(!map || !name)
		return NULL;
	Texture *tex = findtex(map, name);
        if(tex)
                return tex;

        printf("not found: %s, loading\n", name);
	tex = loadtexstruct(name);
        arraypush(map, &tex);
	qsort(map->ptr, map->nmemb, map->size,
	      (int(*)(const void*, const void*))texcmp);
        return tex;
}

void
glinit() {
        glDisable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glShadeModel(GL_SMOOTH);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glClearColor(0.0f, 0.25f, 0.0f, 0.0f);
        glPointSize(4.0f);
        batch_init();
}

int
init() {
        if(SDL_Init(SDL_INIT_VIDEO) < 0)
                goto out;

        const SDL_VideoInfo *vi = SDL_GetVideoInfo();
        screen.fullsize.x = vi->current_w;
	screen.fullsize.y = vi->current_h;
	screen.bpp = vi->vfmt->BitsPerPixel;
	screen.size = &screen.winsize;

        screen.format.BitsPerPixel = screen.bpp;
        screen.format.BytesPerPixel = screen.bpp / 8;
        screen.format.Rshift = screen.bpp / 4 * COLOR_SHIFT(3);
        screen.format.Gshift = screen.bpp / 4 * COLOR_SHIFT(2);
        screen.format.Bshift = screen.bpp / 4 * COLOR_SHIFT(1);
        screen.format.Ashift = screen.bpp / 4 * COLOR_SHIFT(0);
        screen.format.Rmask = ((1 <<  screen.bpp / 4) - 1) << screen.format.Rshift;
        screen.format.Gmask = ((1 <<  screen.bpp / 4) - 1) << screen.format.Gshift;
        screen.format.Bmask = ((1 <<  screen.bpp / 4) - 1) << screen.format.Bshift;
        screen.format.Amask = ((1 <<  screen.bpp / 4) - 1) << screen.format.Ashift;

        if(resize(1024, 768, false, Diagonal))
                goto out_sdl;

        SDL_WM_SetCaption("Radmire", NULL);
        SDL_EnableKeyRepeat(0, 0);

	glinit();

        timer.step = 1 / timer.fps;
        timer.clockstep = CLOCKS_PER_SEC / timer.fps;
        timer.prev = clock();

        running = true;
        return 0;

out_sdl:
        SDL_Quit();
out:
        return -2;
}

int
loadatlas(Array *map, const char *spec, const char *image) {
	if(!map || !spec || !image)
		return 0;
	
	FILE *specfile = fopen(spec, "rb");
	if(!spec)
		return 0;

	unsigned int atlas = loadtex(image);

	size_t nprev = map->nmemb;
	Texture *tex;
	Texture *dup;
        int ntex;
        for(ntex = 0; (tex = specline(map, specfile)); ) {
		tex->handle = atlas;
		dup = findntex(map, nprev, tex->name);
		// Ignore this line on duplicate, or we could:
		// memset(dup, tex,
		//        sizeof(Texture) - sizeof(tex->name));
		// freetex(tex);
		// tex = dup;
		if(dup) {
			printf("duplicate found: %s\n", tex->name);
			freetex(tex);
		} else {
			printf("inserting: %s\n", tex->name);
			arraypush(map, &tex);
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

unsigned int
loadtex(const char *path) {
        unsigned int handle = 0;
        SDL_Surface *orig = NULL;
        SDL_Surface *conv = NULL;

        orig = IMG_Load(path);
        if(!orig)
                goto out_orig;

        conv = formatimg(orig);
        if(!conv)
                goto out_conv;

        handle = maketex(conv);

        SDL_FreeSurface(conv);
out_conv:
        SDL_FreeSurface(orig);
out_orig:
        return handle;
}

Texture*
loadtexstruct(const char *path) {
	Texture *tex = NULL;
        SDL_Surface *orig = NULL;
	SDL_Surface *pow2 = NULL;
        SDL_Surface *conv = NULL;
        unsigned int handle = 0;

        orig = IMG_Load(path);
        if(!orig)
                goto out_orig;

	pow2 = pow2img(orig);
	if(!pow2)
		goto out_pow2;

        conv = formatimg(pow2);
        if(!conv)
                goto out_conv;

        handle = maketex(conv);
        SDL_FreeSurface(conv);
	if(!handle)
		goto out_conv;

        tex = malloc(sizeof(tex[0]));

        tex->handle = handle;

	Vec2 s = {
		orig->w / (double)pow2->w,
		orig->h / (double)pow2->h
	};
	tex->coords[0] = (Vec2){0, s.y};
	tex->coords[1] = s;
	tex->coords[2] = (Vec2){s.x, 0};
	tex->coords[3] = vec2zero;

        tex->name = malloc((strlen(path) + 1) * sizeof(path[0]));
        strcpy(tex->name, path);

out_conv:
	if(orig != pow2)
		SDL_FreeSurface(pow2);
out_pow2:
        SDL_FreeSurface(orig);
out_orig:
        return tex;
}

unsigned int
maketex(SDL_Surface *surface) {
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

SDL_Surface*
pow2img(SDL_Surface *src) {
        if(ISPOW2(src->w) && ISPOW2(src->h))
                return src;
        SDL_SetAlpha(src, 0, SDL_ALPHA_OPAQUE);
        SDL_Surface *dst = NULL;
        dst = SDL_CreateRGBSurface(src->flags,
                                   topow2(src->w), topow2(src->h),
                                   src->format->BitsPerPixel,
                                   src->format->Rmask,
                                   src->format->Gmask,
                                   src->format->Bmask,
                                   src->format->Amask);
        if(!dst || SDL_BlitSurface(src, NULL, dst, NULL)) {
                SDL_FreeSurface(dst);
                return NULL;
        }
        return dst;

}

int
resize(int width, int height, bool fullscreen, int base) {
	printf("%dx%d@%d\n", width, height, screen.bpp);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, screen.bpp / 4);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, screen.bpp / 4);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, screen.bpp / 4);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, screen.bpp / 4);

        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

        Uint32 flags = SDL_OPENGL | SDL_RESIZABLE;
	flags |= fullscreenflag(fullscreen);

        if(!SDL_SetVideoMode(width, height, screen.bpp, flags))
                return -1;

        screen.size->x = width;
        screen.size->y = height;
        screen.full = fullscreen;

	if(fullscreen)
		glinit();

        glViewport(0, 0, width, height);
 	setbase[base](width, height);
        screen.base = base;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(screen.left, screen.right, screen.bottom, screen.top);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        screen.tform.col1.x = (screen.right - screen.left) / width;
        screen.tform.col2.y = (screen.bottom - screen.top) / height;
        screen.tform.pos.x = screen.left;
        screen.tform.pos.y = screen.top;
        return 0;
}

void
setbasediagonal(int width, int height) {
        screen.top = screen.right = 100.0f * 2.0f
	                    / sqrtf(width * width + height * height);

        screen.right *= width;
        screen.left = -screen.right;
        screen.top *= height;
        screen.bottom = -screen.top;
}

void
setbasehorizontal(int width, int height) {
        screen.right = 100.0f;
        screen.left = -screen.right;
        screen.top =  100.0f * height / width;
        screen.bottom = -screen.top;
}

void
setbasenone(int width, int height) {
        screen.right = width * 0.5f;
        screen.left = -screen.right;
        screen.top = height * 0.5f;
        screen.bottom = -screen.top;
}

void
setbasevertical(int width, int height) {
        screen.right = 100.0f * width / height;
        screen.left = -screen.right;
        screen.top = 100.0f;
        screen.bottom = -screen.top;
}

int
setfullscreen(int base) {
	screen.size = &screen.fullsize;
        return resize(screen.fullsize.x, screen.fullsize.y, true, base);
}

int
setwindowed(int base) {
	screen.size = &screen.winsize;
        return resize(screen.winsize.x, screen.winsize.y, false, base);
}

void
sleepc(clock_t iv) {
	SDL_Delay(iv * 1000 / CLOCKS_PER_SEC);
}

Texture*
specline(Array *map, FILE *specfile) {
	static char buff[BUFSIZ];
	Texture *tex;
	tex = calloc(1, sizeof(tex[0]));

	int nread = 0;
	Vec2 pos;
	Vec2 siz;
	nread = fscanf(specfile, "%lf %lf %lf %lf ",
	               &pos.x, &pos.y, &siz.x, &siz.y);
	if(nread != 4
           || pos.x < 0 || pos.x >= 1 || pos.y < 0 || pos.y >= 1
           || siz.x <= 0 || siz.x > 1 || siz.y <= 0 || siz.y > 1)
		goto free;
	tex->coords[0] = (Vec2){pos.x, pos.y + siz.y};
	tex->coords[1] = VEC2PLUS(pos, siz);
	tex->coords[2] = (Vec2){pos.x + siz.x, pos.y};
	tex->coords[3] = pos;

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

int
strtexcmp(const char *a, const Texture **b) {
	// FIXME: is this possible:
	if(!a || !b)
		return (a ? 1 : 0) - (b ? 1 : 0);

	return strcmp(a, (*b)->name);
}

Tform
tformfromvec2(const Vec2 v) {       
        Tform res;
        coord len = VEC2LEN(v);
        coord c = v.x / len;
        coord s = v.y / len;

        res.col1.x = c; res.col2.x = -s; res.pos.x = len;
        res.col1.y = s; res.col2.y = c; res.pos.y = 0;

	return res;
}

Tform
tforminv(Tform t) {
	coord det = t.col1.x * t.col2.y - t.col1.y * t.col2.x;
	if(det != 0.0f)
		det = 1.0f / det;

	return (Tform){ { det * t.col2.y, -det * t.col2.x },
	                {-det * t.col1.y,  det * t.col1.x },
	                { det * (t.pos.y * t.col2.x - t.col2.y * t.pos.x),
	                 -det * (t.pos.y * t.col1.x - t.col1.y * t.pos.x) } };
}

void
tformsetangle(Tform *t, coord angle) {       
        coord c = cosc(angle);
        coord s = sinc(angle);
	t->col1.x = c; t->col2.x = -s;
	t->col1.y = s; t->col2.y = c;
}

int
texcmp(const Texture **a, const Texture **b) {
	// FIXME: is this possible:
	if(!a || !b)
		return (a ? 1 : 0) - (b ? 1 : 0);

	return strcmp((*a)->name, (*b)->name);
}

uint32_t
topow2(uint32_t v) {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        return ++v;
}

int
main(int argc, char **argv) {
        if(init())
                return -1;

	loadatlas(&texmap, "atlas/target.atlas", "atlas/target.png");
	Texture *tex = gettex(&texmap, "ball.png");

        Object mouse = {
                tformidentity,
                { 12, 12 }
        };
        
        Object ball = {
                tformidentity,
                { 32, 32 }
        };
        
        Vec2 line[] = {
                { -100, -75 },
                { 100, -75 }
	};

        Vec2 cross[] = {
                { -5, 5 },
                { 5, -5 },
                { -5, -5 },
                { 5, 5 }
        };

	Tform world = tformidentity;
	Tform angle = tformidentity;
	Tform walk = tformidentity;
	coord dist = 0;

        while(running) {
                beginframe();

		walk.pos.x = -dist;
		tformsetangle(&angle, input.mouse.screenabs.y * 0.01f);
		world = TFORMMUL(angle, walk);
		walk.pos.x = dist;

                if(input.keyboard.pressed[SDLK_ESCAPE])
                        running = false;
		if(PRESSING_KEY(SDLK_f))
			setfullscreen(Diagonal);
		if(PRESSING_KEY(SDLK_w))
			setwindowed(Diagonal);
                if(input.keyboard.pressed[SDLK_RIGHT])
			dist++;
                if(input.keyboard.pressed[SDLK_LEFT])
			dist--;
                if(PRESSING_BTN(1)) {
                        Object new = {
                                tformidentity,
                                {50, 50}
                        };
                        new.t.pos = TFORMVEC2(TFORMT(tforminv(world)), input.mouse.screenabs);
                        arraypush(&objects, &new);
                }
                beginscene();

                batch_bind_texture(tex->handle);
		batch.texcoords = tex->coords;
                Object *p = objects.ptr;
                for(int i = 0; i < objects.nmemb; i++) {
                        batch.tform = TFORMMUL(world, p[i].t);
                        batch_draw_rect(&p[i].s, 0);
                }

		batch.texcoords = tex->coords;
                batch.tform = TFORMMUL(world, ball.t);
                batch_draw_rect(&ball.s, 0);

		batch.texcoords = tex->coords;
                mouse.t.pos = input.mouse.screenabs;
                batch.tform = mouse.t;
                batch_draw_rect(&mouse.s, 0);

                batch.vertices = line;
                batch.tform = world;
                batch_draw_arrays(GL_LINES, 0, LENGTH(line));

                batch.vertices = cross;
                batch.tform = TFORMMUL(world, walk);
                batch_draw_arrays(GL_LINES, 0, LENGTH(cross));

                endscene();
                endframe();
        }

        freetexmap(&texmap);

        SDL_Quit();
        return 0;
}

