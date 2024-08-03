#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef struct {
    float x, y;
} Vec2f;

typedef struct {
    float x, y, z;
} Vec3f;

typedef struct {
    float x, y, z, w;
} Vec4f;

/***************************/

typedef struct {
    int a, b, c;
} Face;

typedef struct {
    Vec2f points[3];
} Triangle;

Vec3f rotateX(Vec3f v, float angle);
Vec3f rotateY(Vec3f v, float angle);
Vec3f rotateZ(Vec3f v, float angle);

/********************************/
/*      VECTOR OPERATIONS       */
/********************************/
float vec2Length(Vec2f v);
float vec3Length(Vec3f v);

Vec2f vec2add(Vec2f v1, Vec2f v2);
Vec3f vec3add(Vec3f v1, Vec3f v2);
Vec4f vec4add(Vec4f v1, Vec4f v2);

Vec2f vec2sub(Vec2f v1, Vec2f v2);
Vec3f vec3sub(Vec3f v1, Vec3f v2);
Vec4f vec4sub(Vec4f v1, Vec4f v2);

Vec2f vec2mult(Vec2f v, float k);
Vec3f vec3mult(Vec3f v, float k);
Vec4f vec4mult(Vec4f v, float k);


Vec2f vec2div(Vec2f v, float k);
Vec3f vec3div(Vec3f v, float k);
Vec4f vec4div(Vec4f v, float k);

Vec2f vec2normalize(Vec2f v);
Vec3f vec3normalize(Vec3f v);
Vec4f vec4normalize(Vec4f v);

void vec3normalizeRef(Vec3f *v);

float vec2dot(Vec2f v1, Vec2f v2);
float vec3dot(Vec3f v1, Vec3f v2);
float vec4dot(Vec4f v1, Vec4f v2);

Vec3f vec3cross(Vec3f v1, Vec3f v2);
// Vec4f vec4cross(Vec4f v1, Vec4f v2);
/********************************/
/*              DEBUG           */
/********************************/
void printVec(Vec3f v);

#endif // GEOMETRY_H