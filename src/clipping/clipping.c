#include "./clipping.h"

void initializeFrustumPlanes(float fov, float znear, float zfar) {
    float cosFov = cos(fov / 2);
    float sinFov = sin(fov / 2);

    Vec3f origin = {0, 0, 0};

    frustumPlanes[LEFT_PLANE].point = origin;
    frustumPlanes[LEFT_PLANE].normal = (Vec3f){cosFov, 0, sinFov};
    
    frustumPlanes[RIGHT_PLANE].point = origin;
    frustumPlanes[RIGHT_PLANE].normal = (Vec3f){-cosFov, 0, sinFov};

    frustumPlanes[BOTTOM_PLANE].point = origin;
    frustumPlanes[BOTTOM_PLANE].normal = (Vec3f){0, cosFov, sinFov};

    frustumPlanes[TOP_PLANE].point = origin;
    frustumPlanes[TOP_PLANE].normal = (Vec3f){0, -cosFov, sinFov};

    frustumPlanes[NEAR_PLANE].point = (Vec3f){0, 0, znear};
    frustumPlanes[NEAR_PLANE].normal = (Vec3f){0, 0, 1};

    frustumPlanes[FAR_PLANE].point = (Vec3f){0, 0, zfar};
    frustumPlanes[FAR_PLANE].normal = (Vec3f){0, 0, -1};

}