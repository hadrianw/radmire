#include "rr_types.h"
#include <math.h>

static inline unsigned int to_pow2(unsigned int v)
{
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        return ++v;
}

static inline struct RRvec2 rr_vec2_plus(struct RRvec2 v1, struct RRvec2 v2)
{
        struct RRvec2 tmp;
        tmp.x = v1.x + v2.x;
        tmp.y = v1.y + v2.y;
        return tmp;
}

static inline struct RRvec2 rr_vec2_minus(struct RRvec2 v1, struct RRvec2 v2)
{
        struct RRvec2 tmp;
        tmp.x = v1.x - v2.x;
        tmp.y = v1.y - v2.y;
        return tmp;
}

static inline RRfloat rr_vec2_sqlen(struct RRvec2 v)
{
        return v.x*v.x + v.y*v.y;
}

static inline struct RRvec2 rr_transform_vect(const struct RRtransform t,
                const struct RRvec2 v)
{       
        struct RRvec2 res;
	res.x = t.pos.x+t.col1.x*v.x+t.col2.x*v.y;
	res.y = t.pos.y+t.col1.y*v.x+t.col2.y*v.y;

	return res;
}

static inline struct RRvec2 rr_transformR_vect(const struct RRtransform t,
                const struct RRvec2 v)
{       
        struct RRvec2 res;
	res.x = t.col1.x*v.x+t.col2.x*v.y;
	res.y = t.col1.y*v.x+t.col2.y*v.y;

	return res;
}

