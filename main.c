#include <assert.h>
#include <math.h>
#include <physfs.h>
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
#include "contrib/physfsrwops.h"

/* macros */
#define LENGTH(X) (sizeof(X) / sizeof (X)[0])
#define RR_DOUBLE_FLOAT
#define RR_SDL_MAX_BUTTONS 255
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define COLOR_SHIFT(X) (3 - (X))
#else
#define COLOR_SHIFT(X) (X)
#endif
#ifdef RR_DOUBLE_FLOAT
#define RRGL_FLOAT_TYPE GL_DOUBLE
#else
#define RRGL_FLOAT_TYPE GL_FLOAT
#endif
#define ispow2(X) (((X) & ((X) - 1)) == 0)
#define BATCH_VERTS 16384

#define LOG_FATAL(fmt, ...) rr_fatal(__FILE__, __LINE__, __func__, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) rr_error(__FILE__, __LINE__, __func__, fmt, __VA_ARGS__)
#define LOG_WARN(fmt, ...) rr_warn(__FILE__, __LINE__, __func__, fmt, __VA_ARGS__)
#define LOG_INFO(fmt, ...) rr_info(fmt, __VA_ARGS__)

#ifdef RR_DOUBLE_FLOAT
#define rr_sqrt sqrt
#define rr_sin sin
#define rr_cos cos
#define rr_asin asin
#define rr_acos acos
#define rr_atan2 atan2
#else
#define rr_sqrt srtf
#define rr_sin sinf
#define rr_cos cosf
#define rr_asin asinf
#define rr_acos acosf
#define rr_atan2 atan2f
#endif

enum RR_ASPECT_BASE {
        RR_DIAGONAL,
        RR_VERTICAL,
        RR_NONE_BASE,
        RR_HORIZONTAL
};

#ifdef RR_DOUBLE_FLOAT
typedef double RRfloat;
#else
typedef float RRfloat;
#endif

struct RRVec2 {
        RRfloat x;
        RRfloat y;
};

struct RRTform {
        struct RRVec2 col1;
        struct RRVec2 col2;
        struct RRVec2 pos;
};

struct RRColor {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha;
};
struct RRArray {
        void *ptr;
        size_t nalloc;
        size_t nmemb;
        size_t size;
};

struct RRTex {
        unsigned int handle;
        struct RRVec2 coords[4];
	char *name;
};
struct Object {
        struct RRTform t;
        struct RRVec2 s;
};

/* function declarations */
static unsigned int to_pow2(unsigned int v);
static void rrarray_resize(struct RRArray *array, size_t nmemb);

static size_t rrarray_push(struct RRArray *array, void *src);

// removes element at index
// moves last element to removed element
// returns index of moved element or/and size of resized array
static size_t rrarray_remove(struct RRArray *array, size_t index);

static size_t rrarray_remove2(struct RRArray *array, size_t index);

static void rrarray_set(struct RRArray *array, size_t nmemb, size_t size);

static void rrarray_free(struct RRArray *array);
void rr_fatal(const char *file, int line, const char *function, const char *format, ...);
void rr_error(const char *file, int line, const char *function, const char *format, ...);
void rr_warn(const char *file, int line, const char *function, const char *format, ...);
void rr_info(const char *format, ...);
static SDL_Surface *rr_loadimg(const char *path);
static SDL_Surface *rr_formatimg(SDL_Surface *src);
static struct RRTex *rr_loadrrtex(const char *path);
static unsigned int rr_loadtex(const char *path);
static unsigned int rr_maketex(SDL_Surface *surface);
static SDL_Surface *rr_pow2img(SDL_Surface *src);

static int rr_addatlas(struct RRArray *map, const char *spc, const char *img);
static struct RRTex *rr_gettex(struct RRArray *map, const char *name);
static struct RRTex *rr_findtex(struct RRArray *map, const char *name);
static struct RRTex *rr_findntex(struct RRArray *map, size_t nel, const char *name);
static void rr_freemaptex(struct RRArray *map);
static void rrgl_init(void);

static void rrgl_vertex_pointer(struct RRVec2 *pointer);
static void rrgl_color_pointer(struct RRColor *pointer);
static void rrgl_texcoord_pointer(struct RRVec2 *pointer);
static void rrgl_draw_arrays(GLenum mode, GLint first, GLsizei count);
static void rrgl_draw_elements(GLenum mode, GLsizei count, const unsigned int *indices);

static void rrgl_draw_rect(const struct RRVec2 *size, const struct RRVec2 *align);


static void rrgl_init(void);

static void rrgl_vertex_pointer(struct RRVec2 *pointer);
static void rrgl_color_pointer(struct RRColor *pointer);
static void rrgl_texcoord_pointer(struct RRVec2 *pointer);
static void rrgl_draw_arrays(GLenum mode, GLint first, GLsizei count);
static void rrgl_draw_elements(GLenum mode, GLsizei count, const unsigned int *indices);

static void rrgl_draw_rect(const struct RRVec2 *size, const struct RRVec2 *align);

static void rrgl_flush(void);

static void rrgl_color(struct RRColor color);
static void rrgl_load_tform(const struct RRTform *t);
static void rrgl_bind_texture(GLuint texture);
extern void rr_sleep(clock_t iv);
static void rr_set_base_diagonal(int width, int height);
static void rr_set_base_vertical(int width, int height);
static void rr_set_base_none(int width, int height);
static void rr_set_base_horizontal(int width, int height);

static void rr_set_screen_tform(int width, int height, RRfloat left, RRfloat right, RRfloat bottom, RRfloat top);
static void rr_resize(int width, int height, int base);
static int rr_fullscreen_mode(int base);
static int rr_set_video_mode(int width, int height, int bpp, bool fullscreen, int base);

static void rr_begin_frame(void);
static void rr_begin_scene(void);
static void rr_end_scene(void);
static void rr_end_frame(void);
static int rr_init(int argc, char **argv);
static void rr_deinit(void);

/* variables */
struct RRArray rr_map = {
	.size = sizeof(struct RRTex*)
};

static const struct RRVec2 rr_vec2_zero = {0.0f, 0.0f};

static const struct RRVec2 rr_vec2_top_left      = {0.0f, 0.0f};
static const struct RRVec2 rr_vec2_top_center    = {0.5f, 0.0f};
static const struct RRVec2 rr_vec2_top_right     = {1.0f, 0.0f};

static const struct RRVec2 rr_vec2_middle_left   = {0.0f, 0.5f};
static const struct RRVec2 rr_vec2_center        = {0.5f, 0.5f};
static const struct RRVec2 rr_vec2_middle_right  = {1.0f, 0.5f};

static const struct RRVec2 rr_vec2_bottom_left   = {0.0f, 1.0f};
static const struct RRVec2 rr_vec2_bottom_center = {0.5f, 1.0f};
static const struct RRVec2 rr_vec2_bottom_right  = {1.0f, 1.0f};

static const struct RRTform rr_tform_identity = {
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f}
};

static const struct RRColor rr_white = {0xFF, 0xFF, 0xFF, 0xFF};
static const struct RRColor rr_magenta = {0xFF, 0x00, 0xFF, 0xFF};
static const struct RRColor rr_red = {0xFF, 0x00, 0x00, 0xFF};
static const struct RRColor rr_green = {0x00, 0xFF, 0x00, 0xFF};

static const struct RRVec2 rr_texcoords_identity[4] = {
	{0.0f, 1.0f},
	{1.0f, 1.0f},
	{1.0f, 0.0f},
	{0.0f, 0.0f}
};

static struct RRVec2 *vertices = NULL;
static struct RRColor *colors = NULL;
static struct RRVec2 *texcoords = NULL;

static struct RRVec2 batch_vertices[BATCH_VERTS];
static struct RRColor batch_colors[BATCH_VERTS];
static struct RRVec2 batch_texcoords[BATCH_VERTS];
static unsigned int batch_count = 0;
static GLenum batch_mode = GL_QUADS;

static struct RRTform tform;
static struct RRColor color;

static GLuint active_texture = 0;

static int rr_width = -1;
static int rr_height = -1;
static int rr_bpp = -1;
static SDL_PixelFormat rr_format = {
        .alpha = 255
};
static bool rr_fullscreen = false;

static RRfloat rr_top;
static RRfloat rr_left;
static RRfloat rr_bottom;
static RRfloat rr_right;

static int rr_base;

static struct RRTform rr_screen_tform;
static RRfloat rr_width_factor = 0.0f;
static RRfloat rr_height_factor = 0.0f;

static bool rr_pressed_keys[SDLK_LAST] = { false };
static bool rr_changed_keys[SDLK_LAST] = { false };
static bool rr_pressed_buttons[RR_SDL_MAX_BUTTONS] = { false };
static bool rr_changed_buttons[RR_SDL_MAX_BUTTONS] = { false };
static struct RRVec2 rr_abs_mouse = {0.0f, 0.0f};
static struct RRVec2 rr_rel_mouse = {0.0f, 0.0f};
static struct RRVec2 rr_abs_screen_mouse = {0.0f, 0.0f};
static struct RRVec2 rr_rel_screen_mouse = {0.0f, 0.0f};
static bool rr_mouse_moved = false;
static bool rr_key_pressed = false;
static bool rr_button_pressed = false;
static bool rr_key_released = false;
static bool rr_button_released = false;

static bool rr_running = false;
static SDL_Event rr_sdl_event;
static struct RRArray objects = {
        .size = sizeof(struct Object)
};
static void (*set_base_handler[])(int, int) = {
	[RR_DIAGONAL] = rr_set_base_diagonal,
        [RR_VERTICAL] = rr_set_base_vertical,
        [RR_NONE_BASE] = rr_set_base_none,
        [RR_HORIZONTAL] = rr_set_base_horizontal
};
static unsigned int rr_fps = 60;
static RRfloat rr_step = 0;
static clock_t rr_time_step;
static clock_t rr_time;
static clock_t rr_time_prev;
static clock_t rr_time_diff;
static int ticks = 0;


/* function implementations */
void rrarray_resize(struct RRArray *array, size_t nmemb)
{
        size_t nalloc = to_pow2(nmemb);
        if(nmemb < array->nmemb && array->nalloc > nalloc * 2)
                nalloc *= 2;
        array->ptr = realloc(array->ptr, nalloc * array->size);
        array->nalloc = nalloc;
        array->nmemb = nmemb;
}

size_t rrarray_push(struct RRArray *array, void *src)
{
        size_t last = array->nmemb * array->size;
        rrarray_resize(array, array->nmemb + 1);
        memcpy((char*)array->ptr + last, src, array->size);
        return last;
}

// removes element at index
// moves last element to removed element
// returns index of moved element or/and size of resized array
size_t rrarray_remove(struct RRArray *array, size_t index)
{
        size_t last = array->nmemb - 1;
        if(last != index) {
                char *ptr = array->ptr;
                index *= array->size;
                last *= array->size;
                memcpy(ptr + index, ptr + last, array->size);
        }
        rrarray_resize(array, array->nmemb - 1);
        return array->nmemb;
}

size_t rrarray_remove2(struct RRArray *array, size_t index)
{
        char *ptr = array->ptr;
        ptr += index * array->size;
        memmove(ptr, ptr + array->size, (array->nmemb - index - 1) * array->size);
        rrarray_resize(array, array->nmemb - 1);
        return array->nmemb;
}

void rrarray_set(struct RRArray *array, size_t nmemb, size_t size)
{
        array->size = size;
        rrarray_resize(array, nmemb);
}

void rrarray_free(struct RRArray *array)
{
        free(array->ptr);
        free(array);
}

void rr_fatal(const char *file, int line, const char *function, const char *format, ...) {
	va_list argp;
	fprintf(stderr, "%s: In `%s`:\n%s:%d: fatal runtime error: ", file, function, file, line);
	va_start(argp, format);
	vfprintf(stderr, format, argp);
	va_end(argp);
	fprintf(stderr, "\n");
}

void rr_error(const char *file, int line, const char *function, const char *format, ...) {
	va_list argp;
	fprintf(stderr, "%s: In `%s`:\n%s:%d: runtime error: ", file, function, file, line);
	va_start(argp, format);
	vfprintf(stderr, format, argp);
	va_end(argp);
	fprintf(stderr, "\n");
}

void rr_warn(const char *file, int line, const char *function, const char *format, ...) {
	va_list argp;
	fprintf(stderr, "%s: In `%s`:\n%s:%d: runtime warning: ", file, function, file, line);
	va_start(argp, format);
	vfprintf(stderr, format, argp);
	va_end(argp);
	fprintf(stderr, "\n");
}

void rr_info(const char *format, ...) {
	va_list argp;
	fprintf(stderr, " ");
	va_start(argp, format);
	vfprintf(stderr, format, argp);
	va_end(argp);
	fprintf(stderr, "\n");
}
unsigned int to_pow2(unsigned int v)
{
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        return ++v;
}

static inline struct RRVec2 rr_vec2(RRfloat x, RRfloat y)
{
	struct RRVec2 v = { x, y };
	return v;
}

static inline struct RRVec2 rr_vec2_plus(struct RRVec2 v1, struct RRVec2 v2)
{
        struct RRVec2 tmp;
        tmp.x = v1.x + v2.x;
        tmp.y = v1.y + v2.y;
        return tmp;
}

static inline struct RRVec2 rr_vec2_minus(struct RRVec2 v1, struct RRVec2 v2)
{
        struct RRVec2 tmp;
        tmp.x = v1.x - v2.x;
        tmp.y = v1.y - v2.y;
        return tmp;
}

static inline RRfloat rr_vec2_sqlen(struct RRVec2 v)
{
        return v.x*v.x + v.y*v.y;
}

static inline RRfloat rr_vec2_len(struct RRVec2 v)
{
        return rr_sqrt(rr_vec2_sqlen(v));
}

static inline struct RRVec2 rr_tform_vect(const struct RRTform t,
                const struct RRVec2 v)
{       
        struct RRVec2 res;
	res.x = t.pos.x+t.col1.x*v.x+t.col2.x*v.y;
	res.y = t.pos.y+t.col1.y*v.x+t.col2.y*v.y;

	return res;
}

static inline struct RRTform rr_tform_mul(const struct RRTform a,
                const struct RRTform b)
{       
        struct RRTform res;
	res.col1.x = a.col1.x * b.col1.x + a.col2.x * b.col1.y;
	res.col2.x = a.col1.x * b.col2.x + a.col2.x * b.col2.y;
	res.pos.x = a.col1.x * b.pos.x + a.col2.x * b.pos.y + a.pos.x;

	res.col1.y = a.col1.y * b.col1.x + a.col2.y * b.col1.y;
	res.col2.y = a.col1.y * b.col2.x + a.col2.y * b.col2.y;
	res.pos.y = a.col1.y * b.pos.x + a.col2.y * b.pos.y + a.pos.y;
	return res;
}

static inline void rr_tform_set_angle(struct RRTform *t,
                RRfloat angle)
{       
        RRfloat c = rr_cos(angle);
        RRfloat s = rr_sin(angle);
	t->col1.x = c; t->col2.x = -s;
	t->col1.y = s; t->col2.y = c;
}

static inline struct RRTform rr_tform_from_vec2(const struct RRVec2 v)
{       
        struct RRTform res;
        RRfloat len = rr_vec2_len(v);
        RRfloat c = v.x / len;
        RRfloat s = v.y / len;

        res.col1.x = c; res.col2.x = -s; res.pos.x = len;
        res.col1.y = s; res.col2.y = c; res.pos.y = 0;

	return res;
}

static inline struct RRVec2 rr_tformR_vect(const struct RRTform t,
                const struct RRVec2 v)
{       
        struct RRVec2 res;
	res.x = t.col1.x*v.x+t.col2.x*v.y;
	res.y = t.col1.y*v.x+t.col2.y*v.y;

	return res;
}
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

struct RRTex *rr_loadrrtex(const char *path)
{
	struct RRTex *tex = NULL;
        SDL_Surface *orig = NULL;
	SDL_Surface *pow2 = NULL;
        SDL_Surface *conv = NULL;
        unsigned int handle = 0;

        orig = rr_loadimg(path);
        if(!orig)
                goto out_orig;

	pow2 = rr_pow2img(orig);
	if(!pow2)
		goto out_pow2;

        conv = rr_formatimg(pow2);
        if(!conv)
                goto out_conv;

        handle = rr_maketex(conv);
        SDL_FreeSurface(conv);
	if(!handle)
		goto out_conv;

        tex = malloc(sizeof(tex[0]));

        tex->handle = handle;

	struct RRVec2 s = {
		orig->w / (double)pow2->w,
		orig->h / (double)pow2->h
	};
	tex->coords[0] = rr_vec2(0, s.y);
	tex->coords[1] = s;
	tex->coords[2] = rr_vec2(s.x, 0);
	tex->coords[3] = rr_vec2_zero;

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

SDL_Surface *rr_pow2img(SDL_Surface *src)
{
        if(ispow2(src->w) && ispow2(src->h))
                return src;
        SDL_SetAlpha(src, 0, SDL_ALPHA_OPAQUE);
        SDL_Surface *dst = NULL;
        dst = SDL_CreateRGBSurface(src->flags,
                                   to_pow2(src->w), to_pow2(src->h),
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
	struct RRVec2 pos;
	struct RRVec2 siz;
	nread = fscanf(specfile, "%lf %lf %lf %lf ",
	               &pos.x, &pos.y, &siz.x, &siz.y);
	if(nread != 4
           || pos.x < 0 || pos.x >= 1 || pos.y < 0 || pos.y >= 1
           || siz.x <= 0 || siz.x > 1 || siz.y <= 0 || siz.y > 1)
		goto free;
	tex->coords[0] = rr_vec2(pos.x, pos.y + siz.y);
	tex->coords[1] = rr_vec2_plus(pos, siz);
	tex->coords[2] = rr_vec2(pos.x + siz.x, pos.y);
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

        printf("not found: %s, loading\n", name);
	tex = rr_loadrrtex(name);
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
void rrgl_init(void)
{
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
}

void rrgl_vertex_pointer(struct RRVec2 *pointer)
{
        vertices = pointer;
}

void rrgl_color_pointer(struct RRColor *pointer)
{
        colors = pointer;
}

void rrgl_texcoord_pointer(struct RRVec2 *pointer)
{
        texcoords = pointer;
}

void rrgl_draw_arrays(GLenum mode, GLint first, GLsizei count)
{
        unsigned int i;
        if(count > BATCH_VERTS) {
                LOG_ERROR("count: %d > BATCH_VERTS: %d", count, BATCH_VERTS);
                return;
        }
        if(batch_mode != mode || batch_count + count > BATCH_VERTS) {
                rrgl_flush();
                batch_mode = mode;
        }
        for(i = 0; i < count; ++i)
                batch_vertices[batch_count + i]
                        = rr_tform_vect(tform, vertices[first + i]);
        if(colors)
                memcpy(batch_colors + batch_count, colors + first,
                                sizeof(*colors) * count);
        else
                for(i = 0; i < count; ++i)
                        batch_colors[batch_count + i] = color;
        if(texcoords)
                memcpy(batch_texcoords + batch_count, texcoords + first,
                                sizeof(*texcoords) * count);

        batch_count += count;
}

void rrgl_draw_elements(GLenum mode, GLsizei count, const unsigned int *indices)
{
        unsigned int i;
        if(count > BATCH_VERTS) {
                LOG_ERROR("count: %d > BATCH_VERTS: %d", count, BATCH_VERTS);
                return;
        }
        if(batch_mode != mode || batch_count + count > BATCH_VERTS) {
                rrgl_flush();
                batch_mode = mode;
        }
        for(i = 0; i < count; ++i)
                batch_vertices[batch_count + i]
                        = rr_tform_vect(tform, vertices[indices[i]]);
        if(colors)
                for(i = 0; i < count; ++i)
                        batch_colors[batch_count + i]
                                = colors[indices[i]];
        else
                for(i = 0; i < count; ++i)
                        batch_colors[batch_count + i] = color;
        if(texcoords)
                for(i = 0; i < count; ++i)
                        batch_texcoords[batch_count + i]
                                = texcoords[indices[i]];

        batch_count += count;
}

void rrgl_draw_rect(const struct RRVec2 *size, const struct RRVec2 *align)
{
        if(!size)
                return;

        if(!align)
                align = &rr_vec2_center;
        /* 3 ,--, 2
             | /|
             |/ |
           0 '--' 1 */
        struct RRVec2 vs[4] = {
                {size->x *       - align->x , size->y * (align->y - 1.0f)},
                {size->x * (1.0f - align->x), size->y * (align->y - 1.0f)},
                {size->x * (1.0f - align->x), size->y *  align->y        },
                {size->x *       - align->x , size->y *  align->y        }
        };
        static const unsigned int is[2 * 3] = {0, 2, 3, 0, 1, 2};

        rrgl_vertex_pointer(vs);
        rrgl_color_pointer(NULL);
        rrgl_draw_elements(GL_TRIANGLES, LENGTH(is), is);
        rrgl_vertex_pointer(NULL);
}

void rrgl_flush()
{
        glVertexPointer(2, RRGL_FLOAT_TYPE, 0, batch_vertices);
        glTexCoordPointer(2, RRGL_FLOAT_TYPE, 0, batch_texcoords);
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, batch_colors);

        if(batch_mode < GL_TRIANGLES)
                glDisable(GL_TEXTURE_2D);
        glDrawArrays(batch_mode, 0, batch_count);
        if(batch_mode < GL_TRIANGLES)
                glEnable(GL_TEXTURE_2D);
        batch_count = 0;
}

void rrgl_color(struct RRColor c)
{
        color = c;
}

void rrgl_load_tform(const struct RRTform *t)
{
        tform = *t;
}

void rrgl_bind_texture(GLuint texture)
{
        if(active_texture == texture)
                return;

	rrgl_flush();
        glBindTexture(GL_TEXTURE_2D, texture);
        active_texture = texture;
}

static inline bool rr_pressing_key(unsigned int key)
{
        return rr_pressed_keys[key] && rr_changed_keys[key];
}

static inline bool rr_pressing_button(unsigned int btn)
{
        return rr_pressed_buttons[btn] && rr_changed_buttons[btn];
}

void rr_set_base_diagonal(int width, int height)
{
        rr_top = rr_right = 100.0f*2.0f/sqrtf(width*width+height*height);

        rr_right *= width;
        rr_left = -rr_right;
        rr_top *= height;
        rr_bottom = -rr_top;
}

void rr_set_base_vertical(int width, int height)
{
        rr_right = 100.0f*width/height;
        rr_left = -rr_right;
        rr_top = 100.0f;
        rr_bottom = -rr_top;
}

void rr_set_base_none(int width, int height)
{
        rr_right = width*0.5f;
        rr_left = -rr_right;
        rr_top = height*0.5f;
        rr_bottom = -rr_top;
}

void rr_set_base_horizontal(int width, int height)
{
        rr_right = 100.0f;
        rr_left = -rr_right;
        rr_top =  100.0f*height/width;
        rr_bottom = -rr_top;
}

void rr_set_screen_tform(int width, int height, RRfloat left, RRfloat right, RRfloat bottom, RRfloat top)
{
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(left, right, bottom, top);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        rr_width_factor = (right-left)/width;
        rr_height_factor = (bottom-top)/height;
        rr_screen_tform = rr_tform_identity;
        rr_screen_tform.col1.x = rr_width_factor;
        rr_screen_tform.col2.y = rr_height_factor;
        rr_screen_tform.pos.x = left;
        rr_screen_tform.pos.y = top;
}

void rr_resize(int width, int height, int base)
{
        glViewport(0, 0, width, height);

 	set_base_handler[base](width, height);
        rr_base = base;
        rr_set_screen_tform(width, height, rr_left, rr_right, rr_bottom, rr_top);
}

int rr_fullscreen_mode(int base)
{
        const SDL_VideoInfo *vi = SDL_GetVideoInfo();
        return rr_set_video_mode(vi->current_w, vi->current_h, vi->vfmt->BitsPerPixel, true, base);
}


int rr_set_video_mode(int width, int height, int bpp, bool fullscreen, int base)
{
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, bpp / 4);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, bpp / 4);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, bpp / 4);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, bpp / 4);

        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

        Uint32 flags = SDL_OPENGL | SDL_RESIZABLE;
        if(fullscreen)
                flags |= SDL_FULLSCREEN;

        if(!SDL_SetVideoMode(width, height, bpp, flags))
                return -1;

        rr_format.BitsPerPixel = bpp;
        rr_format.BytesPerPixel = bpp / 8;
        rr_format.Rshift = bpp / 4 * COLOR_SHIFT(3);
        rr_format.Gshift = bpp / 4 * COLOR_SHIFT(2);
        rr_format.Bshift = bpp / 4 * COLOR_SHIFT(1);
        rr_format.Ashift = bpp / 4 * COLOR_SHIFT(0);
        rr_format.Rmask = ((1 <<  bpp / 4) - 1) << rr_format.Rshift;
        rr_format.Gmask = ((1 <<  bpp / 4) - 1) << rr_format.Gshift;
        rr_format.Bmask = ((1 <<  bpp / 4) - 1) << rr_format.Bshift;
        rr_format.Amask = ((1 <<  bpp / 4) - 1) << rr_format.Ashift;

        rr_width = width;
        rr_height = height;
        rr_bpp = bpp;
        rr_fullscreen = fullscreen;

        rr_resize(rr_width, rr_height, base);
        return 0;
}

void rr_begin_frame(void)
{
        for(unsigned int i=0; i < SDLK_LAST; ++i)
                rr_changed_keys[i] = false;
        for(unsigned int i=0; i < RR_SDL_MAX_BUTTONS; ++i)
                rr_changed_buttons[i] = false;
        rr_rel_mouse = rr_vec2_zero;
        rr_mouse_moved = false;
        rr_key_pressed = false;
        rr_button_pressed = false;
        rr_key_released = false;
        rr_button_released = false;
        while(SDL_PollEvent(&rr_sdl_event)) {
                switch(rr_sdl_event.type) {
                case SDL_QUIT:
                        rr_running = false;
                        return;
                case SDL_MOUSEMOTION:
                        rr_rel_mouse.x += rr_sdl_event.motion.xrel;
                        rr_rel_mouse.y += rr_sdl_event.motion.yrel;
                        rr_abs_mouse.x = rr_sdl_event.motion.x;
                        rr_abs_mouse.y = rr_sdl_event.motion.y;
                        rr_mouse_moved = true;
                        break;
                case SDL_MOUSEBUTTONDOWN:
                        rr_pressed_buttons[rr_sdl_event.button.button-1] = true;
                        rr_changed_buttons[rr_sdl_event.button.button-1] = true;
                        rr_button_pressed = true;
                        break;
                case SDL_MOUSEBUTTONUP:
                        rr_pressed_buttons[rr_sdl_event.button.button-1] = false;
                        rr_changed_buttons[rr_sdl_event.button.button-1] = true;
                        rr_button_released = true;
                        break;
                case SDL_KEYDOWN:
                        rr_pressed_keys[rr_sdl_event.key.keysym.sym] = true;
                        rr_changed_keys[rr_sdl_event.key.keysym.sym] = true;
                        rr_key_pressed = true;
                        break;
                case SDL_KEYUP:
                        rr_pressed_keys[rr_sdl_event.key.keysym.sym] = false;
                        rr_changed_keys[rr_sdl_event.key.keysym.sym] = true;
                        rr_key_released = true;
                        break;
                case SDL_VIDEORESIZE:
                        rr_set_video_mode(rr_sdl_event.resize.w,
                                        rr_sdl_event.resize.h, rr_bpp,
                                        rr_fullscreen, rr_base);
                        break;
                }
        }
        rr_abs_screen_mouse = rr_tform_vect(rr_screen_tform,
                        rr_abs_mouse); 
        rr_rel_screen_mouse = rr_tformR_vect(rr_screen_tform,
                        rr_rel_mouse); 
}

void rr_begin_scene(void)
{
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
}

void rr_end_scene(void)
{
        rrgl_flush();
        glFlush();
        SDL_GL_SwapBuffers();
}

void rr_end_frame(void)
{
        rr_time = clock();
        rr_time_diff = rr_time - rr_time_prev;

        if(rr_time_diff < rr_time_step) {
                rr_time_diff = rr_time_step - rr_time_diff;
		rr_sleep(rr_time_diff);
                ticks++;
                if(ticks == rr_fps) {
                        printf("%f\n", rr_time_diff / (double)CLOCKS_PER_SEC);
                        ticks = 0;
                }
        }

        rr_time_prev = rr_time;
}

int rr_init(int argc, char **argv)
{
	if(!PHYSFS_init(argv[0]))
                goto out;
	if(!PHYSFS_setSaneConfig("hawski", "rr", "zip", 0, 0))
                goto out_physfs;

        if(SDL_Init(SDL_INIT_VIDEO) < 0)
                goto out_physfs;
        if(rr_set_video_mode(1024, 768, 32, false, RR_DIAGONAL))
        //if(rr_fullscreen_mode(RR_DIAGONAL))
                goto out_sdl;

        SDL_WM_SetCaption("Radmire", NULL);
        SDL_EnableKeyRepeat(0, 0);
        glDisable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glShadeModel(GL_SMOOTH);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glClearColor(0.0f, 0.25f, 0.0f, 0.0f);
        glPointSize(4.0f);
        rrgl_init();
        rrgl_color(rr_white);
        rrgl_load_tform(&rr_tform_identity);

        rr_step = 1 / rr_fps;
        rr_time_step = CLOCKS_PER_SEC / rr_fps;
        rr_time_prev = clock();

        rr_running = true;
        return 0;

out_sdl:
        SDL_Quit();
out_physfs:
        PHYSFS_deinit();
out:
        return -2;
}

void rr_deinit(void)
{
        SDL_Quit();
        PHYSFS_deinit();
}

int main(int argc, char **argv)
{
        if(rr_init(argc, argv)) {
                return -1;
        }

	rr_addatlas(&rr_map, "atlas/target.atlas", "atlas/target.png");
	struct RRTex *tex = rr_gettex(&rr_map, "ball.png");

        struct Object mouse = {
                rr_tform_identity,
                { 12, 12 }
        };
        
        struct Object ball = {
                rr_tform_identity,
                { 32, 32 }
        };
        
        struct RRVec2 line[] = {
                { -100, -70 },
                { 100, -75 }
        };

        while(rr_running) {
                rr_begin_frame();
                if(rr_pressed_keys[SDLK_ESCAPE])
                        rr_running = false;
                if(rr_changed_buttons[1] && rr_pressed_buttons[1]) {
                        struct Object new = {
                                rr_tform_identity,
                                {50, 50}
                        };
                        new.t.pos = rr_abs_screen_mouse;
                        rrarray_push(&objects, &new);
                }
                rr_begin_scene();

                rrgl_bind_texture(tex->handle);
		rrgl_texcoord_pointer(tex->coords);
                struct Object *p = objects.ptr;
                for(int i = 0; i < objects.nmemb; i++) {
                        rrgl_load_tform(&p[i].t);
                        rrgl_draw_rect(&p[i].s, 0);
                }

		rrgl_texcoord_pointer(tex->coords);
                mouse.t.pos = rr_abs_screen_mouse;
                rrgl_load_tform(&mouse.t);
                rrgl_draw_rect(&mouse.s, 0);

		rrgl_texcoord_pointer(tex->coords);
                rrgl_load_tform(&ball.t);
                rrgl_draw_rect(&ball.s, 0);

                rrgl_vertex_pointer(line);
                rrgl_load_tform(&rr_tform_identity);
                rrgl_draw_arrays(GL_LINES, 0, LENGTH(line));

                rr_end_scene();
                rr_end_frame();
        }

        rr_freemaptex(&rr_map);
        rr_deinit();
        return 0;
}

