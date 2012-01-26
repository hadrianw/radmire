#include <GL/gl.h>
#include <stdio.h>
#include "rr.h"
#include "rrgl.h"
#include "rr_math.h"
#include "rr_image.h"

int main(int argc, char **argv)
{
        if(rr_init(argc, argv)) {
                return -1;
        }

        unsigned int handle = rrteximg_load("square.png");

        while(rr_running) {
                rr_begin_frame();
                if(rr_pressed_keys[SDLK_ESCAPE])
                        rr_running = false;
                rr_begin_scene();

                rrgl_bind_texture(handle);
                rrgl_draw_rect(&rr_abs_screen_mouse, 0);

                rr_end_scene();
                rr_end_frame();
        }

        rr_deinit();
        return 0;
}

