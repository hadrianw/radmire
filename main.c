#include <GL/gl.h>
#include <stdio.h>
#include "rr.h"
#include "rrgl.h"
#include "rr_math.h"
#include "rr_image.h"

struct RRArray {
        void *ptr;
        size_t nalloc;
        size_t nmemb;
        size_t size;
};

void rrarray_resize(struct RRArray *array, size_t nmemb)
{
        size_t nalloc = to_pow2(nmemb);
        if(nmemb < array->nmemb)
                nalloc *= 2;
        array->ptr = realloc(array->ptr, nalloc * array->size);
        array->nalloc = nalloc;
        array->nmemb = nmemb;
}

size_t rrarray_push(struct RRArray *array, void *src)
{
        size_t last = array->nmemb;
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
                memcpy(ptr + index, ptr + last, array->size);
        }
        rrarray_resize(array, array->nmemb - 1);
        return array->nmemb;
}

struct RRArray *rrarray(size_t nmemb, size_t size)
{
        struct RRArray *array = calloc(nmemb, size);
        array->size = size;
        rrarray_resize(array, nmemb);
        return array;
}

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

