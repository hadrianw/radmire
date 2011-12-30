#include "rr_types.h"

const struct RRvec2 rr_vec2_zero = {0.0f, 0.0f};

const struct RRtransform rr_transform_identity = {
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f}
};

const struct RRcolor rr_white = {0xFF, 0xFF, 0xFF, 0xFF};
const struct RRcolor rr_magenta = {0xFF, 0x00, 0xFF, 0xFF};
const struct RRcolor rr_red = {0xFF, 0x00, 0x00, 0xFF};
const struct RRcolor rr_green = {0x00, 0xFF, 0x00, 0xFF};

