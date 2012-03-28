#include <SDL/SDL_opengl.h>
#include <stdio.h>
#include "rr.h"
#include <chipmunk.h>

#include "rrphysics.h"

#include "utils.h"

struct Object {
        struct RRtransform t;
        struct RRvec2 s;
};

struct RRArray objects = {
        .size = sizeof(struct Object)
};

cpVect grav = { 0, -50 };
cpSpace *space;
cpShape *ground;
cpBody *ballBody;
cpShape *ballShape;

void physics_init()
{
        space = cpSpaceNew();
        cpSpaceSetGravity(space, grav);

        ground = cpSegmentShapeNew(space->staticBody, cpv(-75, -70), cpv(75, -75), 0);
        cpShapeSetFriction(ground, 1);
        cpSpaceAddShape(space, ground);

        cpFloat radius = 16;
        cpFloat mass = 1;
        cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

        ballBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
        cpBodySetPos(ballBody, cpv(0, 0));

        ballShape = cpSpaceAddShape(space, cpCircleShapeNew(ballBody, radius, cpvzero));
        cpShapeSetFriction(ballShape, 0.7);
}

void physics_deinit()
{
        cpShapeFree(ballShape);
        cpBodyFree(ballBody);
        cpShapeFree(ground);
        cpSpaceFree(space);
}

int main(int argc, char **argv)
{
        if(rr_init(argc, argv)) {
                return -1;
        }

	rr_addatlas(&rr_map, "atlas/target.atlas", "atlas/target.png");
	struct RRTex *tex = rr_gettex(&rr_map, "ball.png");
	//struct RRTex *tex = rr_gettex(&rr_map, "square.png");

        struct Object mouse = {
                rr_transform_identity,
                { 12, 12 }
        };
        
        struct Object ball = {
                rr_transform_identity,
                { 32, 32 }
        };
        
        physics_init();

        struct RRvec2 line[] = {
                { -75, -70 },
                { 75, -75 }
        };

        while(rr_running) {
                rr_begin_frame();
                if(rr_pressed_keys[SDLK_ESCAPE])
                        rr_running = false;
                if(rr_changed_buttons[0] && rr_pressed_buttons[0]) {
                        struct Object new = {
                                rr_transform_identity,
                                {50, 50}
                        };
                        new.t.pos = rr_abs_screen_mouse;
                        rrarray_push(&objects, &new);
                }

                rr_begin_scene();

                rrgl_bind_texture(tex->handle);
		rrgl_texcoord_pointer(tex->texcoords);
                struct Object *p = objects.ptr;
                for(int i = 0; i < objects.nmemb; i++) {
                        rrgl_load_transform(&p[i].t);
                        rrgl_draw_rect(&p[i].s, 0);
                }
                mouse.t.pos = rr_abs_screen_mouse;
                rrgl_load_transform(&mouse.t);
                rrgl_draw_rect(&mouse.s, 0);

                ball.t = cp2rr_bodytransform(ballBody);
                rrgl_load_transform(&ball.t);
                rrgl_draw_rect(&ball.s, 0);

                rrgl_vertex_pointer(line);
                rrgl_load_transform(&rr_transform_identity);
                rrgl_draw_arrays(GL_LINES, 0, LENGTH(line));

                rr_end_scene();
                cpSpaceStep(space, 1.0/60.0);
                rr_end_frame();
        }

        physics_deinit();
        rr_freemaptex(&rr_map);
        rr_deinit();
        return 0;
}

