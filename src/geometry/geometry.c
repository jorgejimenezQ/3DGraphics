#include "geometry.h"
#include <math.h>


Vec3f rotateX(Vec3f v, float angle) {
    Vec3f rotatedVector = {
        .x = v.x,
        .y = v.y * cos(angle) - v.z * sin(angle),
        .z = v.y * sin(angle) + v.z * cos(angle)
    };   

    return rotatedVector;
}

Vec3f rotateY(Vec3f v, float angle) {
    Vec3f rotatedVector = {
        .x = v.x * cos(angle) - v.z * sin(angle),
        .y = v.y,
        .z = v.x * sin(angle) + v.z * cos(angle)
    };   

    return rotatedVector;
}
Vec3f rotateZ(Vec3f v, float angle) {
    Vec3f rotatedVector = {
        .x = v.x * cos(angle) - v.y * sin(angle),
        .y = v.x * sin(angle) + v.y * cos(angle),
        .z = v.z
    };   

    return rotatedVector;

}