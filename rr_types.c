#include "rr_types.h"

const struct RRVec2 rr_vec2_zero = {0.0f, 0.0f};

const struct RRVec2 rr_vec2_top_left      = {0.0f, 0.0f};
const struct RRVec2 rr_vec2_top_center    = {0.5f, 0.0f};
const struct RRVec2 rr_vec2_top_right     = {1.0f, 0.0f};

const struct RRVec2 rr_vec2_middle_left   = {0.0f, 0.5f};
const struct RRVec2 rr_vec2_center        = {0.5f, 0.5f};
const struct RRVec2 rr_vec2_middle_right  = {1.0f, 0.5f};

const struct RRVec2 rr_vec2_bottom_left   = {0.0f, 1.0f};
const struct RRVec2 rr_vec2_bottom_center = {0.5f, 1.0f};
const struct RRVec2 rr_vec2_bottom_right  = {1.0f, 1.0f};

const struct RRTform rr_tform_identity = {
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f}
};

const struct RRcolor rr_white = {0xFF, 0xFF, 0xFF, 0xFF};
const struct RRcolor rr_magenta = {0xFF, 0x00, 0xFF, 0xFF};
const struct RRcolor rr_red = {0xFF, 0x00, 0x00, 0xFF};
const struct RRcolor rr_green = {0x00, 0xFF, 0x00, 0xFF};

