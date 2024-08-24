#include "geometry.h"



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

/**************************************************************/
/*                    VECTOR OPERATIONS                       */
/**************************************************************/
// LENGTH
float vec2Length(Vec2f v) {
    return sqrt(v.x*v.x + v.y*v.y);
}

float vec3Length(Vec3f v) {
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

float vec4Length(Vec4f v) {
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}
// ADD 
Vec2f vec2add(Vec2f v1, Vec2f v2) {
    Vec2f result = {
        .x = v1.x + v2.x,
        .y = v1.y + v2.y
    };
    return result;
}

Vec3f vec3add(Vec3f v1, Vec3f v2) {
    Vec3f result = {
        .x = v1.x + v2.x,
        .y = v1.y + v2.y,
        .z = v1.z + v2.z
    };
    return result;
}

Vec4f vec4add(Vec4f v1, Vec4f v2) {
    Vec4f result = {
        .x = v1.x - v2.x,
        .y = v1.y - v2.y,
        .z = v1.z - v2.z,
        .w = v1.w - v2.w,
    };
    return result;
}

// SUBTRACT
Vec2f vec2sub(Vec2f v1, Vec2f v2) {
    Vec2f result = {
        .x = v1.x - v2.x,
        .y = v1.y - v2.y
    };
    return result;
}

Vec3f vec3sub(Vec3f v1, Vec3f v2) {
    Vec3f result = {
        .x = v1.x - v2.x,
        .y = v1.y - v2.y,
        .z = v1.z - v2.z
    };
    return result;
}

Vec4f vec4sub(Vec4f v1, Vec4f v2) {
    Vec4f result = {
        .x = v1.x - v2.x,
        .y = v1.y - v2.y,
        .z = v1.z - v2.z,
        .w = v1.w - v2.w,
    };
    return result;
}

// SCALE/MULTIPLY
Vec2f vec2mult(Vec2f v, float scalar) {
    Vec2f result = {
        .x = v.x * scalar,
        .y = v.y * scalar,
    };
    return result;
}

Vec3f vec3mult(Vec3f v, float scalar) {
    Vec3f result = {
        .x = v.x * scalar,
        .y = v.y * scalar,
        .z = v.z * scalar,
    };
    return result;
}

Vec4f vec4mult(Vec4f v, float scalar) {
    Vec4f result = {
        .x = v.x * scalar,
        .y = v.y * scalar,
        .z = v.z * scalar,
        .w = v.w * scalar,
    };
    return result;
}

Vec2f vec2div(Vec2f v, float scalar) {
    if (scalar == 0) {
        return v;
    }

    Vec2f result = {
        .x = v.x / scalar,
        .y = v.y / scalar,
    };
    return result;
}

Vec3f vec3div(Vec3f v, float scalar) {
    if (scalar == 0) {
        return v;
    }

    Vec3f result = {
        .x = v.x / scalar,
        .y = v.y / scalar,
        .z = v.z / scalar,
    };
    return result;
}

Vec4f vec4div(Vec4f v, float scalar) {
    if (scalar == 0) {
        return v;
    }

    Vec4f result = {
        .x = v.x / scalar,
        .y = v.y / scalar,
        .z = v.z / scalar,
        .w = v.w / scalar,
    };
    return result;
}
// NORMALIZE
Vec2f vec2normalize(Vec2f v) {
    float length = vec2Length(v);
    Vec2f result = {
        .x = v.x / length,
        .y = v.y / length
    };

    return result;
}

Vec3f vec3normalize(Vec3f v) {
    float length = vec3Length(v);
    Vec3f result = {
        .x = v.x / length,
        .y = v.y / length,
        .z = v.z / length
    };
    return result;
}

Vec4f vec4normalize(Vec4f v) {
    float n = vec4Length(v);
    Vec4f result = {
        .x = v.x / n,
        .y = v.y / n,
        .z = v.z / n,
        .w = v.w / n
    };
    return result;
}

void vec3normalizeRef(Vec3f *v) {
    float length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    v->x /= length;
    v->y /= length;
    v->z /= length;
}

// CROSS PRODUCT
Vec3f vec3cross(Vec3f a, Vec3f b) {
    Vec3f result = {
        .x = a.y*b.z - a.z*b.y,
        .y = a.z*b.x - a.x*b.z,
        .z = a.x*b.y - a.y*b.x
    };
    return result;
}

// DOT PRODUCT
float vec2dot(Vec2f a, Vec2f b) {
    return a.x*b.x + a.y*b.y;
}

float vec3dot(Vec3f a, Vec3f b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

float vec4dot(Vec4f a, Vec4f b) {
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

/**************************************************************/
/*                    VECTOR CREATE                           */
/**************************************************************/
Vec2f createVec2data(float *x) {
    Vec2f result = {
        .x = x[0],
        .y = x[1]
    };
    return result;
}

Vec2f createVec2f(float x, float y) {
    Vec2f result = {
        .x = x,
        .y = y
    };
    return result;
}

Vec3f createVec3data(float *x) {
    Vec3f result = {
        .x = x[0],
        .y = x[1],
        .z = x[2]
    };
    return result;
}
Vec3f createVec3f(float x, float y, float z) {
    Vec3f result = {
        .x = x,
        .y = y,
        .z = z
    };
    return result;
}


Vec4f createVec4data(float *x) {
    Vec4f result = {
        .x = x[0],
        .y = x[1],
        .z = x[2],
        .w = x[3]
    };
    return result;
}

Vec4f createVec4f(float x, float y, float z, float w) {
    Vec4f result = {
        .x = x,
        .y = y,
        .z = z,
        .w = w
    };
    return result;
}


Vec2f vec2clone(Vec2f v) {
    Vec2f result = {
        .x = v.x,
        .y = v.y
    };
    return result;
}
Vec3f vec3clone(Vec3f v) {
    Vec3f result = {
        .x = v.x,
        .y = v.y,
        .z = v.z
    };
    return result;
}
Vec4f vec4clone(Vec4f v) {
    Vec4f result = {
        .x = v.x,
        .y = v.y,
        .z = v.z,
        .w = v.w
    };
    return result;
}
/**************************************************************/
/*                         DEBUG                              */
/**************************************************************/

void printVec(Vec3f v) {
    printf("(%f, %f, %f)\n", v.x, v.y, v.z);
}