#ifndef CLIPPING_H
#define CLIPPING_H

// Include any necessary headers here
#include "../geometry/geometry.h"
#include "../draw/draw.h"
#include <math.h>

#define MAX_NUM_POLY_VERTICES 100

typedef struct {
    Vec3f point;
    Vec3f normal;
} Plane;

// 
extern DrawingContext drawingContext;
extern Plane frustumPlanes[6];

enum FrustumPlane {
    LEFT_PLANE,
    RIGHT_PLANE,
    BOTTOM_PLANE,
    TOP_PLANE,
    NEAR_PLANE,
    FAR_PLANE
};

typedef struct {
    Vec3f vertices[MAX_NUM_POLY_VERTICES];
    int numVertices;
} Polygon;


void initializeFrustumPlanes(float fov, float znear, float zfar);
Polygon createPolygonFromTriangle(Vec3f v0, Vec3f v1, Vec3f v2);
void clipPolygon(Polygon *polygon);
void setDrawingContext_clipping(DrawingContext *dc);
#endif // CLIPPING_H