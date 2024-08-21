#ifndef CLIPPING_H
#define CLIPPING_H

// Include any necessary headers here
#include "../geometry/geometry.h"
extern const Point frustumPlanes[6];

enum FrustumPlane {
    LEFT_PLANE,
    RIGHT_PLANE,
    BOTTOM_PLANE,
    TOP_PLANE,
    NEAR_PLANE,
    FAR_PLANE
};

typedef struct {
    Vec3f point;
    Vec3f normal;
} Point;


void initializeFrustumPlanes(float fov, float znear, float zfar);

#endif // CLIPPING_H