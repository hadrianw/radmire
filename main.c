#include <GL/gl.h>
#include <stdio.h>
#include "rr.h"
#include "rr_math.h"
#include "rr_image.h"

int main(int argc, char **argv)
{
        if(rr_init(argc, argv)) {
                return -1;
        }

        SDL_Surface *s = rrimg_load("square.png");
        SDL_Surface *z = rrimg_display_format(s);
        SDL_FreeSurface(s);
        s = z;
        glEnable(GL_TEXTURE_2D);
        unsigned int handle = rrtex_load(s);
        printf("handle %d\n", handle);
        SDL_FreeSurface(s);

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

                /*
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
                */

                glActiveTexture(handle);
                glBegin(GL_QUADS);
                glColor3ub(0xFF, 0xFF, 0);
                glTexCoord2f(0, 1);
                glVertex2i(-100, -100);
                glColor3ub(0xFF, 0xFF, 0xFF);
                glTexCoord2f(0, 0);
                glVertex2i(-100, 100);
                glTexCoord2f(1, 0);
                glVertex2i(100, 100);
                glTexCoord2f(1, 1);
                glVertex2i(100, -100);
                glEnd();

                rr_end_scene();
                rr_end_frame();
        }

        rr_deinit();
        return 0;
}

