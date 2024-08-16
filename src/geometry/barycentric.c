#include "./geometry.h"

int barycentric(Vec2f a, Vec2f b, Vec2f c, Vec2f p, Vec3f *uvw) {
    Vec2f v0 = vec2sub(b, a);
    Vec2f v1 = vec2sub(c, a);
    Vec2f v2 = vec2sub(p, a);

    float d00 = vec2dot(v0, v0);
    float d01 = vec2dot(v0, v1);
    float d11 = vec2dot(v1, v1);
    float d20 = vec2dot(v2, v0);
    float d21 = vec2dot(v2, v1);

    float denom = d00*d11 - d01*d01;

    if (denom == 0) {
        return -1;
    } 

    uvw->y = (d11*d20 - d01*d21)/denom; // v
    uvw->z = (d00*d21 - d01*d20)/denom; // w
    uvw->x = 1.0f - uvw->y - uvw->z; // u

    // Vec2f ac = vec2sub(c, a);
    // Vec2f ab = vec2sub(b, a);
    // Vec2f ap = vec2sub(p, a);
    // Vec2f pc = vec2sub(c, p);
    // Vec2f pb = vec2sub(b, p);

    // // Compute the area of the full parallegram/triangle ABC using 2D cross product
    // float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x); // || AC x AB ||

    // if (area_parallelogram_abc == 0) return -1;

    // uvw->x = (pc.x * pb.y - pc.y * pb.x) / area_parallelogram_abc;
    // uvw->y = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;
    // uvw->z = 1.0f - uvw->x - uvw->y;

    return 1;
}