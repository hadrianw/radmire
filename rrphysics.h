static inline struct RRVec2 cp2rr_vec2(cpVect v)
{
        struct RRVec2 rv = { .x = v.x, .y = v.y };
        return rv;
}

static inline struct RRTform cp2rr_bodytform(cpBody *body)
{
        if(!body)
                return rr_tform_identity;

        struct RRTform t = {
                .pos = cp2rr_vec2(cpBodyGetPos(body)),
                .col1 = cp2rr_vec2(cpBodyGetRot(body))
        };
        t.col2 = rr_vec2(-t.col1.y, t.col1.x);
        return t;
}
