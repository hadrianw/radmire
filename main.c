#include <SDL/SDL_opengl.h>
#include <stdio.h>
#include "rr.h"
#include "rrgl.h"
#include "rr_math.h"
#include "rr_image.h"
#include "rr_array.h"

struct Object {
        struct RRtransform t;
        struct RRvec2 s;
};

struct RRArray objects = {
        .size = sizeof(struct Object)
};

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
                if(rr_changed_buttons[0] && rr_pressed_buttons[0]) {
                        struct Object new = {
                                rr_transform_identity,
                                {5, 5}
                        };
                        new.t.pos = rr_abs_screen_mouse;
                        rrarray_push(&objects, &new);
                }

                rr_begin_scene();

                rrgl_bind_texture(handle);
                struct Object *p = objects.ptr;
                for(int i = 0; i < objects.nmemb; i++) {
                        rrgl_load_transform(&p[i].t);
                        rrgl_draw_rect(&p[i].s, 0);
                }

                rr_end_scene();
                rr_end_frame();
        }

        rr_deinit();
        return 0;
}

