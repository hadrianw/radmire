#include <SDL/SDL.h>
#include "rr_types.h"

#define RR_SDL_MAX_BUTTONS 255

extern const int rr_pf_red_bits[];
extern const int rr_pf_green_bits[];
extern const int rr_pf_blue_bits[];
extern const int rr_pf_alpha_bits[];
extern const int rr_pf_bpp[];

extern int rr_width;
extern int rr_height;
extern int rr_bpp;
extern SDL_PixelFormat rr_format;
extern bool rr_fullscreen;

extern RRfloat rr_top;
extern RRfloat rr_left;
extern RRfloat rr_bottom;
extern RRfloat rr_right;

extern int rr_base;

extern struct RRtransform rr_screen_transform;
extern RRfloat rr_width_factor;
extern RRfloat rr_height_factor;

extern bool rr_pressed_keys[SDLK_LAST];
extern bool rr_changed_keys[SDLK_LAST];
extern bool rr_pressed_buttons[RR_SDL_MAX_BUTTONS];
extern bool rr_changed_buttons[RR_SDL_MAX_BUTTONS];
extern struct RRvec2 rr_abs_mouse;
extern struct RRvec2 rr_rel_mouse;
extern bool rr_mouse_moved;
extern bool rr_key_pressed;
extern bool rr_button_pressed;
extern bool rr_key_released;
extern bool rr_button_released;

extern bool rr_running;

void rr_set_base_diagonal(int width, int height);
void rr_set_base_vertical(int width, int height);
void rr_set_base_none(int width, int height);
void rr_set_base_horizontal(int width, int height);

void rr_set_screen_transform(int width, int height, RRfloat left, RRfloat right, RRfloat bottom, RRfloat top);
void rr_resize(int width, int height, int base);
int rr_set_video_mode(int width, int height, int bpp, bool fullscreen, int base);

inline bool rr_pressing_key(unsigned int key)
{
        return rr_pressed_keys[key] && rr_changed_keys[key];
}

inline bool rr_pressing_button(unsigned int btn)
{
        return rr_pressed_buttons[btn] && rr_changed_buttons[btn];
}

void rr_begin_frame(void);
void rr_begin_scene(void);
void rr_end_scene(void);
void rr_end_frame(void);
int rr_init(int argc, char **argv);
void rr_deinit(void);
