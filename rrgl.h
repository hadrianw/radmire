#include <SDL/SDL_opengl.h>

extern struct RRvec2 rr_texcoords_identity[4];

extern void rrgl_init(void);

extern void rrgl_vertex_pointer(struct RRvec2 *pointer);
extern void rrgl_color_pointer(struct RRcolor *pointer);
extern void rrgl_texcoord_pointer(struct RRvec2 *pointer);
extern void rrgl_draw_arrays(GLenum mode, GLint first, GLsizei count);
extern void rrgl_draw_elements(GLenum mode, GLsizei count, const unsigned int *indices);

extern void rrgl_draw_rect(const struct RRvec2 *size, const struct RRvec2 *align);


extern void rrgl_init(void);

extern void rrgl_vertex_pointer(struct RRvec2 *pointer);
extern void rrgl_color_pointer(struct RRcolor *pointer);
extern void rrgl_texcoord_pointer(struct RRvec2 *pointer);
extern void rrgl_draw_arrays(GLenum mode, GLint first, GLsizei count);
extern void rrgl_draw_elements(GLenum mode, GLsizei count, const unsigned int *indices);

extern void rrgl_draw_rect(const struct RRvec2 *size, const struct RRvec2 *align);

extern void rrgl_flush(void);

extern void rrgl_color(struct RRcolor color);
extern void rrgl_load_transform(const struct RRtransform *t);
extern void rrgl_bind_texture(GLuint texture);
