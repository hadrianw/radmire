#include "rr_types.h"
#include <GL/gl.h>

void rrgl_init(void);

void rrgl_vertex_pointer(struct RRvec2 *pointer);
void rrgl_color_pointer(struct RRcolor *pointer);
void rrgl_texcoord_pointer(struct RRvec2 *pointer);
void rrgl_draw_arrays(GLenum mode, GLint first, GLsizei count);
void rrgl_draw_elements(GLenum mode, GLsizei count, const unsigned int *indices);

void rrgl_flush(void);

void rrgl_color(struct RRcolor color);
void rrgl_load_transform(const struct RRtransform *t);
