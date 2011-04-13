#include "rrgl.h"

struct RRvec2 *vertices = NULL;
struct RRcolor *colors = NULL;
struct RRvec2 *texcoords = NULL;

#define BATCH_VERTS 16384
struct RRvec2 batch_vertices[BATCH_VERTS];
struct RRcolor batch_colors[BATCH_VERTS];
struct RRvec2 batch_texcoords[BATCH_VERTS];
unsigned int batch_vertex_count = 0;
GLenum batch_mode = GL_QUADS;

struct RRtransform transform;
struct RRcolor color;

void rrgl_vertex_pointer(struct RRvec2 *pointer)
{
        vertices = pointer;
}

void rrgl_color_pointer(struct RRcolor *pointer);
{
        colors = pointer;
}

void rrgl_texcoord_pointer(struct RRvec2 *pointer);
{
        texcoords = pointer;
}

void rrgl_draw_arrays(GLenum mode, GLint first, GLsizei count)
{
        unsigned int i = first;
        for(; i < count; ++i) {
                batch_vertices[batch_count+i]
                        = rr_transform_vect(transform, vertices[i]);
        }
        if(colors)
                memcpy(batch_colors
                batch_colors[batch_count++] = color;
        batch_count += count;
}

void rrgl_draw_elements(GLenum mode, GLsizei count, const GLvoid *indices);

void rrgl_color(struct RRcolor *c)
{
        color = *color;
}

void rrgl_load_transform(struct RRtransform *t)
{
        transform = *t;
}
