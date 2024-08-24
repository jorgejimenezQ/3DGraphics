#include "./transformations.h"

void polygonToTriangles(Polygon *polygon, Triangle *trianglesOut) {
    int tIdx = 0;
    for (int i = 2; i < polygon->numVertices; i++) {
        trianglesOut[tIdx].points[0] =  (Vec4f){ polygon->vertices[0].x, polygon->vertices[0].y, polygon->vertices[0].z, 1};
        trianglesOut[tIdx].points[1] =  (Vec4f){ polygon->vertices[i -1].x, polygon->vertices[i -1].y, polygon->vertices[i -1].z, 1};
        trianglesOut[tIdx].points[2] =  (Vec4f){ polygon->vertices[i].x, polygon->vertices[i].y, polygon->vertices[i].z, 1};
        tIdx++;
    }
}