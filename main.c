#include <stdio.h>
#include "radmire.h"

#include "utils.h"

struct Object {
        struct RRTform t;
        struct RRVec2 s;
};

struct RRArray objects = {
        .size = sizeof(struct Object)
};

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

