#include <stdbool.h>
#include <stdint.h>

enum RR_PIXEL_FORMAT {
        RR_NONE_FORMAT,
        RR_A8,
	RR_L8,
	RR_L8A8,
	RR_A8L8,
	RR_R5G6B5,
	RR_R5G5B5A1,
	RR_R8G8B8,
	RR_R8G8B8A8,
	RR_PF_COUNT
};

enum RR_ASPECT_BASE {
        RR_DIAGONAL,
        RR_VERTICAL,
        RR_NONE_BASE,
        RR_HORIZONTAL
};

typedef double RRfloat;

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

struct RRmesh {
        struct RRvec2 *vertices;
        unsigned int allocated;
        unsigned int used;
};

