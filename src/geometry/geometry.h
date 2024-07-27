#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef struct {
    float x;
    float y;
} Vec2f;

typedef struct {
    float x;
    float y;
    float z;
} Vec3f;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} Vec4f;

Vec3f rotateX(Vec3f v, float angle);
Vec3f rotateY(Vec3f v, float angle);
Vec3f rotateZ(Vec3f v, float angle);

#endif // GEOMETRY_H