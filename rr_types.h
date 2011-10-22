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
static const struct RRvec2 rr_vec2_zero = {0.0f, 0.0f};

struct RRtransform {
        struct RRvec2 col1;
        struct RRvec2 col2;
        struct RRvec2 pos;
};
static const struct RRtransform rr_transform_identity = {
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f}
};

struct RRcolor {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha;
};
static const struct RRcolor rr_white = {0xFF, 0xFF, 0xFF, 0xFF};
static const struct RRcolor rr_magenta = {0xFF, 0x00, 0xFF, 0xFF};
static const struct RRcolor rr_red = {0xFF, 0x00, 0x00, 0xFF};

struct RRmesh {
        struct RRvec2 *vertices;
        unsigned int allocated;
        unsigned int used;
};

#endif
