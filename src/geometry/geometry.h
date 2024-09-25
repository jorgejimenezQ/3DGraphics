#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <stdint.h>
#include <math.h>
#include <stdio.h>

typedef struct {
    float x, y;
} Vec2f;

typedef struct {
    float x, y, z;
} Vec3f;

typedef struct {
    float x, y, z, w;
} Vec4f;

typedef struct {
    int x, y, z;
} Vec3i;

/***************************/

typedef struct {
    Vec3f scale;
    Vec3f translation;
    Vec3f rotation;
    Vec3f perspective;
    Vec3f view;
    Vec3f world;
    Vec3f projection;
} Transformations;

Transformations transformationsEmpty();

typedef struct {
    Vec3f points[3];
    Vec3f normals[3];
    Vec2f uvTexture[3];
    Vec3i indices;
    int materialIndex;
} Face;


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
/*       Vector create          */
/********************************/
Vec2f createVec2data(float *x);
Vec2f createVec2f(float x, float y);

Vec3f createVec3data(float *x);
Vec3f createVec3f(float x, float y, float z);

Vec4f createVec4data(float *x);
Vec4f createVec4f(float x, float y, float z, float w);

Vec2f vec2clone(Vec2f v);
Vec3f vec3clone(Vec3f v);
Vec4f vec4clone(Vec4f v);

/********************************/
/*       LIGHT SOURCE           */
/********************************/
typedef struct {
    Vec3f direction;
} Light;

/********************************/
/*        Barycentric           */
/********************************/
// Compute the barycentric coordinates (u, v, w) for
// Point p with respect to triangle (a, b, c)
int barycentric(Vec2f a, Vec2f b, Vec2f c, Vec2f p, Vec3f *uvw);

/********************************/
/*              DEBUG           */
/********************************/
void printVec(Vec3f v);



#endif // GEOMETRY_H