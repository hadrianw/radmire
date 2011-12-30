#ifndef RR_rr_types_h_
#define RR_rr_types_h_
#include <stdbool.h>
#include <stdint.h>

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

struct RRvec2 {
        RRfloat x;
        RRfloat y;
};
extern const struct RRvec2 rr_vec2_zero;

struct RRtransform {
        struct RRvec2 col1;
        struct RRvec2 col2;
        struct RRvec2 pos;
};
extern const struct RRtransform rr_transform_identity;

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
        struct RRvec2 *vertices;
        unsigned int allocated;
        unsigned int used;
};

#endif
