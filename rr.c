#include <time.h>
#include <physfs.h>

extern void rr_sleep(clock_t iv);

#include "rr_types.h"
#include "rr_math.h"
#include "utils.h"
#include "rr.h"
#include "rrgl.h"

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define COLOR_SHIFT(X) (3 - (X))
#else
#define COLOR_SHIFT(X) (X)
#endif

static void (*set_base_handler[])(int, int) = {
	[RR_DIAGONAL] = rr_set_base_diagonal,
        [RR_VERTICAL] = rr_set_base_vertical,
        [RR_NONE_BASE] = rr_set_base_none,
        [RR_HORIZONTAL] = rr_set_base_horizontal
};

int rr_width = -1;
int rr_height = -1;
int rr_bpp = -1;
SDL_PixelFormat rr_format = {
        .alpha = 255
};
bool rr_fullscreen = false;

RRfloat rr_top;
RRfloat rr_left;
RRfloat rr_bottom;
RRfloat rr_right;

int rr_base;

struct RRTform rr_screen_tform;
RRfloat rr_width_factor = 0.0f;
RRfloat rr_height_factor = 0.0f;

bool rr_pressed_keys[SDLK_LAST] = { false };
bool rr_changed_keys[SDLK_LAST] = { false };
bool rr_pressed_buttons[RR_SDL_MAX_BUTTONS] = { false };
bool rr_changed_buttons[RR_SDL_MAX_BUTTONS] = { false };
struct RRVec2 rr_abs_mouse = {0.0f, 0.0f};
struct RRVec2 rr_rel_mouse = {0.0f, 0.0f};
struct RRVec2 rr_abs_screen_mouse = {0.0f, 0.0f};
struct RRVec2 rr_rel_screen_mouse = {0.0f, 0.0f};
bool rr_mouse_moved = false;
bool rr_key_pressed = false;
bool rr_button_pressed = false;
bool rr_key_released = false;
bool rr_button_released = false;

bool rr_running = false;
static SDL_Event rr_sdl_event;

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

extern inline bool rr_pressing_key(unsigned int key);
extern inline bool rr_pressing_button(unsigned int btn);

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

static unsigned int rr_fps = 60;
static RRfloat rr_step = 0;
static clock_t rr_time_step;
static clock_t rr_time;
static clock_t rr_time_prev;
static clock_t rr_time_diff;
static int ticks = 0;

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
	if(!PHYSFS_setSaneConfig("hawski", "rr", "spk", 0, 0))
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
