#include <math.h>

#ifdef RR_DOUBLE_FLOAT
#define rr_sqrt sqrt
#define rr_sin sin
#define rr_cos cos
#define rr_asin asin
#define rr_acos acos
#define rr_atan2 atan2
#else
#define rr_sqrt srtf
#define rr_sin sinf
#define rr_cos cosf
#define rr_asin asinf
#define rr_acos acosf
#define rr_atan2 atan2f
#endif

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

static inline RRfloat rr_vec2_len(struct RRvec2 v)
{
        return rr_sqrt(rr_vec2_sqlen(v));
}

static inline struct RRvec2 rr_transform_vect(const struct RRtransform t,
                const struct RRvec2 v)
{       
        struct RRvec2 res;
	res.x = t.pos.x+t.col1.x*v.x+t.col2.x*v.y;
	res.y = t.pos.y+t.col1.y*v.x+t.col2.y*v.y;

	return res;
}

static inline struct RRtransform rr_transform_mul(const struct RRtransform a,
                const struct RRtransform b)
{       
        struct RRtransform res;
	res.col1.x = a.col1.x * b.col1.x + a.col2.x * b.col1.y;
	res.col2.x = a.col1.x * b.col2.x + a.col2.x * b.col2.y;
	res.pos.x = a.col1.x * b.pos.x + a.col2.x * b.pos.y + a.pos.x;

	res.col1.y = a.col1.y * b.col1.x + a.col2.y * b.col1.y;
	res.col2.y = a.col1.y * b.col2.x + a.col2.y * b.col2.y;
	res.pos.y = a.col1.y * b.pos.x + a.col2.y * b.pos.y + a.pos.y;
	return res;
}

static inline void rr_transform_set_angle(struct RRtransform *t,
                RRfloat angle)
{       
        RRfloat c = rr_cos(angle);
        RRfloat s = rr_sin(angle);
	t->col1.x = c; t->col2.x = -s;
	t->col1.y = s; t->col2.y = c;
}

static inline struct RRtransform rr_transform_from_vec2(const struct RRvec2 v)
{       
        struct RRtransform res;
        RRfloat len = rr_vec2_len(v);
        RRfloat c = v.x / len;
        RRfloat s = v.y / len;

        res.col1.x = c; res.col2.x = -s; res.pos.x = len;
        res.col1.y = s; res.col2.y = c; res.pos.y = 0;

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

