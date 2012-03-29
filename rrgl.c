#include "rr_types.h"
#include "rrgl.h"
#include "rr_math.h"
#include "utils.h"
#include <string.h>

#ifdef RR_DOUBLE_FLOAT
#define RRGL_FLOAT_TYPE GL_DOUBLE
#else
#define RRGL_FLOAT_TYPE GL_FLOAT
#endif

struct RRVec2 rr_texcoords_identity[4] = {
	{0.0f, 1.0f},
	{1.0f, 1.0f},
	{1.0f, 0.0f},
	{0.0f, 0.0f}
};

static struct RRVec2 *vertices = NULL;
static struct RRcolor *colors = NULL;
static struct RRVec2 *texcoords = NULL;

#define BATCH_VERTS 16384
static struct RRVec2 batch_vertices[BATCH_VERTS];
static struct RRcolor batch_colors[BATCH_VERTS];
static struct RRVec2 batch_texcoords[BATCH_VERTS];
static unsigned int batch_count = 0;
static GLenum batch_mode = GL_QUADS;

static struct RRTform tform;
static struct RRcolor color;

static GLuint active_texture = 0;

void rrgl_init(void)
{
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
}

void rrgl_vertex_pointer(struct RRVec2 *pointer)
{
        vertices = pointer;
}

void rrgl_color_pointer(struct RRcolor *pointer)
{
        colors = pointer;
}

void rrgl_texcoord_pointer(struct RRVec2 *pointer)
{
        texcoords = pointer;
}

void rrgl_draw_arrays(GLenum mode, GLint first, GLsizei count)
{
        unsigned int i;
        if(count > BATCH_VERTS) {
                LOG_ERROR("count: %d > BATCH_VERTS: %d", count, BATCH_VERTS);
                return;
        }
        if(batch_mode != mode || batch_count + count > BATCH_VERTS) {
                rrgl_flush();
                batch_mode = mode;
        }
        for(i = 0; i < count; ++i)
                batch_vertices[batch_count + i]
                        = rr_tform_vect(tform, vertices[first + i]);
        if(colors)
                memcpy(batch_colors + batch_count, colors + first,
                                sizeof(*colors) * count);
        else
                for(i = 0; i < count; ++i)
                        batch_colors[batch_count + i] = color;
        if(texcoords)
                memcpy(batch_texcoords + batch_count, texcoords + first,
                                sizeof(*texcoords) * count);

        batch_count += count;
}

void rrgl_draw_elements(GLenum mode, GLsizei count, const unsigned int *indices)
{
        unsigned int i;
        if(count > BATCH_VERTS) {
                LOG_ERROR("count: %d > BATCH_VERTS: %d", count, BATCH_VERTS);
                return;
        }
        if(batch_mode != mode || batch_count + count > BATCH_VERTS) {
                rrgl_flush();
                batch_mode = mode;
        }
        for(i = 0; i < count; ++i)
                batch_vertices[batch_count + i]
                        = rr_tform_vect(tform, vertices[indices[i]]);
        if(colors)
                for(i = 0; i < count; ++i)
                        batch_colors[batch_count + i]
                                = colors[indices[i]];
        else
                for(i = 0; i < count; ++i)
                        batch_colors[batch_count + i] = color;
        if(texcoords)
                for(i = 0; i < count; ++i)
                        batch_texcoords[batch_count + i]
                                = texcoords[indices[i]];

        batch_count += count;
}

void rrgl_draw_rect(const struct RRVec2 *size, const struct RRVec2 *align)
{
        if(!size)
                return;

        if(!align)
                align = &rr_vec2_center;
        /* 3 ,--, 2
             | /|
             |/ |
           0 '--' 1 */
        struct RRVec2 vs[4] = {
                {size->x *       - align->x , size->y * (align->y - 1.0f)},
                {size->x * (1.0f - align->x), size->y * (align->y - 1.0f)},
                {size->x * (1.0f - align->x), size->y *  align->y        },
                {size->x *       - align->x , size->y *  align->y        }
        };
        static const unsigned int is[2 * 3] = {0, 2, 3, 0, 1, 2};

        rrgl_vertex_pointer(vs);
        rrgl_color_pointer(NULL);
        rrgl_draw_elements(GL_TRIANGLES, LENGTH(is), is);
        rrgl_vertex_pointer(NULL);
}

void rrgl_flush()
{
        glVertexPointer(2, RRGL_FLOAT_TYPE, 0, batch_vertices);
        glTexCoordPointer(2, RRGL_FLOAT_TYPE, 0, batch_texcoords);
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, batch_colors);

        if(batch_mode < GL_TRIANGLES)
                glDisable(GL_TEXTURE_2D);
        glDrawArrays(batch_mode, 0, batch_count);
        if(batch_mode < GL_TRIANGLES)
                glEnable(GL_TEXTURE_2D);
        batch_count = 0;
}

void rrgl_color(struct RRcolor c)
{
        color = c;
}

void rrgl_load_tform(const struct RRTform *t)
{
        tform = *t;
}

void rrgl_bind_texture(GLuint texture)
{
        if(active_texture == texture)
                return;

        glBindTexture(GL_TEXTURE_2D, texture);
        active_texture = texture;
}

