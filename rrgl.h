#include <SDL/SDL_opengl.h>

extern void rrgl_init(void);

extern void rrgl_vertex_pointer(struct RRVec2 *pointer);
extern void rrgl_color_pointer(struct RRcolor *pointer);
extern void rrgl_texcoord_pointer(struct RRVec2 *pointer);
extern void rrgl_draw_arrays(GLenum mode, GLint first, GLsizei count);
extern void rrgl_draw_elements(GLenum mode, GLsizei count, const unsigned int *indices);

extern void rrgl_draw_rect(const struct RRVec2 *size, const struct RRVec2 *align);


extern void rrgl_init(void);

extern void rrgl_vertex_pointer(struct RRVec2 *pointer);
extern void rrgl_color_pointer(struct RRcolor *pointer);
extern void rrgl_texcoord_pointer(struct RRVec2 *pointer);
extern void rrgl_draw_arrays(GLenum mode, GLint first, GLsizei count);
extern void rrgl_draw_elements(GLenum mode, GLsizei count, const unsigned int *indices);

extern void rrgl_draw_rect(const struct RRVec2 *size, const struct RRVec2 *align);

extern void rrgl_flush(void);

extern void rrgl_color(struct RRcolor color);
extern void rrgl_load_tform(const struct RRTform *t);
extern void rrgl_bind_texture(GLuint texture);
