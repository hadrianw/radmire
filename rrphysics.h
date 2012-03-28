static inline struct RRvec2 cp2rr_vec2(cpVect v)
{
        struct RRvec2 rv = { .x = v.x, .y = v.y };
        return rv;
}

static inline struct RRtransform cp2rr_bodytransform(cpBody *body)
{
        if(!body)
                return rr_transform_identity;

        struct RRtransform t = {
                .pos = cp2rr_vec2(cpBodyGetPos(body)),
                .col1 = cp2rr_vec2(cpBodyGetRot(body))
        };
        t.col2 = rr_vec2(-t.col1.y, t.col1.x);
        return t;
}
