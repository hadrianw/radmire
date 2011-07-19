#include <GL/gl.h>
#include <stdio.h>
#include "rr.h"
#include "rr_math.h"

int main(int argc, char **argv)
{
        if(rr_init()) {
                return -1;
        }
        struct RRvec2 screen_mouse;
        struct RRvec2 screen_mouse_rel;
        while(rr_running) {
                rr_begin_frame();
                screen_mouse = rr_transform_vect(rr_screen_transform,
                                rr_abs_mouse); 
                screen_mouse_rel = rr_transformR_vect(rr_screen_transform,
                                rr_rel_mouse); 
                if(rr_pressed_keys[SDLK_ESCAPE])
                        rr_running = false;
                rr_begin_scene();

                /*rrgl_vertex_pointer(rr_nodes);
                rrgl_color(white);
                rrgl_draw_elements(GL_LINES, rr_pair_count * 2,
                                (unsigned int*)rr_pairs);
                rrgl_draw_arrays(GL_POINTS, 0, rr_node_count);
                if(rr_node_count > 0) {
                        rrgl_color(magenta);
                        rrgl_draw_arrays(GL_POINTS, rr_current_node, 1);
                        rrgl_color(red);
                        rrgl_draw_arrays(GL_POINTS, rr_root_node, 1);
                }*/

                glColor4ub(0xFF, 0xFF, 0xFF, 0x80);
                glBegin(GL_LINE_STRIP);
                glVertex2i(-100, -100);
                glVertex2i(-100, 100);
                glVertex2i(100, 100);
                glVertex2i(100, -100);
                glVertex2i(-100, -100);
                glEnd();

                glBegin(GL_LINES);
                glVertex2i(-100, 0);
                glVertex2i(100, 0);
                glVertex2i(0, -100);
                glVertex2i(0, 100);
                glEnd();

                rr_end_scene();
                rr_end_frame();
        }

        rr_deinit();
        return 0;
}

