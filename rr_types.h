#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define RR_DOUBLE_FLOAT

enum RR_ASPECT_BASE {
        RR_DIAGONAL,
        RR_VERTICAL,
        RR_NONE_BASE,
        RR_HORIZONTAL
};

#ifdef RR_DOUBLE_FLOAT
typedef double RRfloat;
#else
typedef float RRfloat;
#endif

struct RRVec2 {
        RRfloat x;
        RRfloat y;
};
extern const struct RRVec2 rr_vec2_zero;
extern const struct RRVec2 rr_vec2_top_left;
extern const struct RRVec2 rr_vec2_top_center;
extern const struct RRVec2 rr_vec2_top_right;
extern const struct RRVec2 rr_vec2_middle_left;
extern const struct RRVec2 rr_vec2_center;
extern const struct RRVec2 rr_vec2_middle_right;
extern const struct RRVec2 rr_vec2_bottom_left;
extern const struct RRVec2 rr_vec2_bottom_center;
extern const struct RRVec2 rr_vec2_bottom_right;

struct RRTform {
        struct RRVec2 col1;
        struct RRVec2 col2;
        struct RRVec2 pos;
};
extern const struct RRTform rr_tform_identity;

struct RRcolor {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha;
};
extern const struct RRcolor rr_white;
extern const struct RRcolor rr_magenta;
extern const struct RRcolor rr_red;
extern const struct RRcolor rr_green;

struct RRmesh {
        struct RRVec2 *vertices;
        unsigned int allocated;
        unsigned int used;
};

struct RRArray {
        void *ptr;
        size_t nalloc;
        size_t nmemb;
        size_t size;
};

struct RRTex {
        unsigned int handle;
        struct RRVec2 texcoords[4];
	char *name;
};

extern const struct RRVec2 rr_texcoords_identity[4];
