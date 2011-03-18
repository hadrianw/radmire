#include "utils.h"

#include <chipmunk/chipmunk.h>

#include <SDL/SDL.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include <unistd.h>

#include <stdio.h>
#include <math.h>
#include "rr_types.h"

const int rr_pf_red_bits[] = {
        0, // RR_NONE_FORMAT
        0, // RR_A8,
	0, // RR_L8,
	0, // RR_L8A8,
	0, // RR_A8L8,
	5, // RR_R5G6B5,
	5, // RR_R5G5B5A1,
	8, // RR_R8G8B8,
	8, // RR_R8G8B8A8,
}; 

const int rr_pf_green_bits[] = {
        0, // RR_NONE_FORMAT
        0, // RR_A8,
	0, // RR_L8,
	0, // RR_L8A8,
	0, // RR_A8L8,
	6, // RR_R5G6B5,
	5, // RR_R5G5B5A1,
	8, // RR_R8G8B8,
	8, // RR_R8G8B8A8,
}; 

const int *rr_pf_blue_bits = rr_pf_red_bits;

const int rr_pf_alpha_bits[] = {
        0, // RR_NONE_FORMAT
        0, // RR_A8,
	0, // RR_L8,
	0, // RR_L8A8,
	0, // RR_A8L8,
	5, // RR_R5G6B5,
	5, // RR_R5G5B5A1,
	8, // RR_R8G8B8,
	8, // RR_R8G8B8A8,
}; 

int rr_width = -1;
int rr_height = -1;
int rr_format = 0;
bool rr_fullscreen = false;

RRfloat rr_top;
RRfloat rr_left;
RRfloat rr_bottom;
RRfloat rr_right;

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

int rr_base;

static inline struct RRvec2 rr_transform_vect(const struct RRtransform t, const struct RRvec2 v)
{       
        struct RRvec2 res;
	res.x = t.pos.x+t.col1.x*v.x+t.col2.x*v.y;
	res.y = t.pos.y+t.col1.y*v.x+t.col2.y*v.y;

	return res;
}

struct RRtransform rr_screen_transform;
RRfloat rr_width_factor = 0.0f;
RRfloat rr_height_factor = 0.0f;

void rr_set_screen_transform(int width, int height, RRfloat left, RRfloat right, RRfloat bottom, RRfloat top)
{
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(left, right, bottom, top);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        rr_width_factor = (right-left)/width;
        rr_height_factor = (bottom-top)/height;
        rr_screen_transform = rr_transform_identity;
        rr_screen_transform.col1.x = rr_width_factor;
        rr_screen_transform.col2.y = rr_height_factor;
        rr_screen_transform.pos.x = left;
        rr_screen_transform.pos.y = top;
}

void rr_resize(int width, int height, int base)
{
        glViewport(0, 0, width, height);

        switch(base) {
        case RR_DIAGONAL:
                rr_set_base_diagonal(width, height);
                break;
        case RR_VERTICAL:
                rr_set_base_vertical(width, height);
                break;
        case RR_NONE_BASE:
                rr_set_base_none(width, height);
                break;
        case RR_HORIZONTAL:
                rr_set_base_horizontal(width, height);
                break;
        }
        rr_base = base;
        rr_set_screen_transform(width, height, rr_left, rr_right, rr_bottom, rr_top);
}

int rr_set_video_mode(int width, int height, int format, bool fullscreen, int base)
{
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, rr_pf_red_bits[format]);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, rr_pf_green_bits[format]);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, rr_pf_blue_bits[format]);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, rr_pf_alpha_bits[format]);

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

        if(!SDL_SetVideoMode(width, height, format, flags))
                return -1;

        rr_width = width;
        rr_height = height;
        rr_format = format;
        rr_fullscreen = fullscreen;

        rr_resize(rr_width, rr_height, base);
        return 0;
}

bool rr_pressed_keys[SDLK_LAST];
bool rr_changed_keys[SDLK_LAST];
#define RR_SDL_MAX_BUTTONS 255
bool rr_pressed_buttons[RR_SDL_MAX_BUTTONS];
bool rr_changed_buttons[RR_SDL_MAX_BUTTONS];
struct RRvec2 rr_abs_mouse = {0.0f, 0.0f};
struct RRvec2 rr_rel_mouse = {0.0f, 0.0f};
bool rr_mouse_moved = false;
bool rr_key_pressed = false;
bool rr_button_pressed = false;
bool rr_key_released = false;
bool rr_button_released = false;

bool rr_running = false;
SDL_Event rr_sdl_event;

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
                        rr_rel_mouse.x = rr_sdl_event.motion.xrel;
                        rr_rel_mouse.y = rr_sdl_event.motion.yrel;
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
                                        rr_sdl_event.resize.h, rr_format,
                                        rr_fullscreen, rr_base);
                        break;
                }
        }
}


void rr_begin_scene(void)
{
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
}


void rr_end_scene(void)
{
        glFlush();
        SDL_GL_SwapBuffers();
}


void rr_end_frame(void)
{
}

#define RR_BATCH_SIZE 4096
struct RRvec2 rr_vertices[RR_BATCH_SIZE];
unsigned int rr_vertex_count = 0;
struct RRvec2 rr_tex_coords[RR_BATCH_SIZE];
unsigned int rr_tex_coords_count = 0;
struct RRcolor rr_colors[RR_BATCH_SIZE];
unsigned int rr_colors_count = 0;

unsigned int rr_batch_count = 0;

void rr_batch_init(void)
{
        glEnableClientState(GL_VERTEX_ARRAY);
        //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        //glEnableClientState(GL_COLOR_ARRAY);

        //glTexCoordPointer(2, GL_DOUBLE, 0, rr_tex_coords);
        //glColorPointer(4, GL_UNSIGNED_BYTE, 0, rr_colors);
}

GLenum rr_polygon_mode = GL_QUADS;

void rr_flush(void)
{
        glVertexPointer(2, GL_DOUBLE, 0, rr_vertices);
        glDrawArrays(rr_polygon_mode, 0, rr_batch_count);
        //rr_batch_count = 0;
}

int rr_init(void)
{
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                return -1;
        }
        if (rr_set_video_mode(1024, 768, RR_R8G8B8A8, false, RR_DIAGONAL))
                goto out_sdl;

        SDL_EnableKeyRepeat(0, 0);
        for(unsigned int i=0; i < SDLK_LAST; ++i)
                rr_pressed_keys[i] = false;
        for(unsigned int i=0; i < RR_SDL_MAX_BUTTONS; ++i)
                rr_pressed_buttons[i] = false;
        rr_batch_init();
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glShadeModel(GL_SMOOTH);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glClearColor(0.0f, 0.25f, 0.0f, 0.0f);

        rr_running = true;
        LOG_INFO("run!");
        return 0;

out_sdl:
        SDL_Quit();
        return -2;
}

void rr_deinit(void)
{
        SDL_Quit();
}


int main(int argc, char **argv)
{
        if(rr_init()) {
                return -1;
        }

        while(rr_running) {
                rr_begin_frame();
                if (rr_pressed_keys[SDLK_ESCAPE])
                        rr_running = false;
                if(rr_pressed_buttons[0] && rr_changed_buttons[0]) {
                        rr_vertices[rr_batch_count] = rr_transform_vect(rr_screen_transform, rr_abs_mouse);
                        ++rr_batch_count;
                }
                rr_begin_scene();

                rr_flush();
                rr_end_scene();
                rr_end_frame();
                usleep(100000);
        }

        rr_deinit();
        return 0;
}
