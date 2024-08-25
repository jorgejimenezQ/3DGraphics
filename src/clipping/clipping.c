#include "./clipping.h"

Plane frustumPlanes[6];
DrawingContext drawingContext;

void initializeFrustumPlanes(float fovX, float fovY, float znear, float zfar) {
    // 
    float cosFovX = cos(fovX / 2);
    float sinFovX = sin(fovX / 2);
    float cosFovY = cos(fovY / 2);
    float sinFovY = sin(fovY / 2);

    Vec3f origin = {0, 0, 0};

    frustumPlanes[LEFT_PLANE].point = origin;
    frustumPlanes[LEFT_PLANE].normal = (Vec3f){cosFovX, 0, sinFovX};
    
    frustumPlanes[RIGHT_PLANE].point = origin;
    frustumPlanes[RIGHT_PLANE].normal = (Vec3f){-cosFovX, 0, sinFovX};

    frustumPlanes[BOTTOM_PLANE].point = origin;
    frustumPlanes[BOTTOM_PLANE].normal = (Vec3f){0, cosFovY, sinFovY};

    frustumPlanes[TOP_PLANE].point = origin;
    frustumPlanes[TOP_PLANE].normal = (Vec3f){0, -cosFovY, sinFovY};

    frustumPlanes[NEAR_PLANE].point = (Vec3f){0, 0, znear};
    frustumPlanes[NEAR_PLANE].normal = (Vec3f){0, 0, 1};

    frustumPlanes[FAR_PLANE].point = (Vec3f){0, 0, zfar};
    frustumPlanes[FAR_PLANE].normal = (Vec3f){0, 0, -1};

}


Polygon createPolygonFromTriangle(Vec3f v0, Vec3f v1, Vec3f v2, Vec2f uv0, Vec2f uv1, Vec2f uv2) {
    Polygon poly;
    poly.vertices[0] = v0;
    poly.vertices[1] = v1;
    poly.vertices[2] = v2;
    poly.uvTextures[0] = uv0;
    poly.uvTextures[1] = uv1;
    poly.uvTextures[2] = uv2;
    poly.numVertices = 3;
    return poly;
}

void clipPolygonAgainstPlane(Polygon *polygon, int plane) {
    Vec3f planePoint = frustumPlanes[plane].point;
    Vec3f planeNormal = frustumPlanes[plane].normal;

    // The array of inside vertices that will be part of the final polygon
    Vec3f insideVertices[MAX_NUM_POLY_VERTICES];
    Vec2f insideUvTextures[MAX_NUM_POLY_VERTICES];
    int numInsideVertices = 0;

    // Start current and previous vertex with the first and last polygon vertices and texture coordinates
    Vec3f *current = &polygon->vertices[0];
    Vec2f *currentUvTexture = &polygon->uvTextures[0];

    Vec3f *previous = &polygon->vertices[polygon->numVertices - 1];
    Vec2f *previousUvTexture = &polygon->uvTextures[polygon->numVertices - 1];

    float currentDot;
    float previousDot = vec3dot(vec3sub(*previous, planePoint), planeNormal);

    while (current != &polygon->vertices[polygon->numVertices]) {
        currentDot = vec3dot(vec3sub(*current, planePoint), planeNormal);

        // If we changed from inside to outside or vice-versa
        if (currentDot * previousDot < 0) {
            // Calculate the interpolation factor, t = dotQ1 / (dotQ1 - dotQ2)
            float t = previousDot / (previousDot - currentDot);
            // Calculate the intersection point I = Q1 + t(Q2-Q1)
            Vec3f q2q1 = vec3sub(*current, *previous);
            q2q1 = vec3mult(q2q1, t);
            Vec3f intersectionPoint = vec3add(*previous, q2q1);

            // Calculate and add the new uv texture coordinates
            Vec2f uv;
            Vec2f uvSub = vec2sub(*currentUvTexture, *previousUvTexture);   
            uvSub = vec2mult(uvSub, t);
            uv = vec2add(*previousUvTexture, uvSub);
            insideUvTextures[numInsideVertices] = uv;
            
            // Insert the new intersection point in the list of "inside vertices"
            insideVertices[numInsideVertices] = vec3clone(intersectionPoint);
            numInsideVertices++;
        }

        if (currentDot > 0) {
            insideVertices[numInsideVertices] = vec3clone(*current);
            insideUvTextures[numInsideVertices] = vec2clone(*currentUvTexture);
            numInsideVertices++;
        }
        // Move to the next vertex
        previousDot = currentDot;
        previous = current;
        previousUvTexture = currentUvTexture;
        current++;
        currentUvTexture++;
    }

    // Copy all the vertices from the insideVertices into the destination polygon
    for (int i = 0; i < numInsideVertices; i++) {
        polygon->vertices[i] = vec3clone(insideVertices[i]);
        polygon->uvTextures[i] = vec2clone(insideUvTextures[i]);
    }
    polygon->numVertices = numInsideVertices;
}

void clipPolygon(Polygon *polygon) {
    clipPolygonAgainstPlane(polygon, LEFT_PLANE);
    clipPolygonAgainstPlane(polygon, RIGHT_PLANE);
    clipPolygonAgainstPlane(polygon, BOTTOM_PLANE);
    clipPolygonAgainstPlane(polygon, TOP_PLANE);
    clipPolygonAgainstPlane(polygon, NEAR_PLANE);
    clipPolygonAgainstPlane(polygon, FAR_PLANE);
}

void setDrawingContext_clipping(DrawingContext *dc) {
    drawingContext.boundX = dc->boundX;
    drawingContext.boundY = dc->boundY;
    drawingContext.buffer = dc->buffer;
    drawingContext.bufferWidth = dc->bufferWidth;
}


int polygonToTriangles(Polygon *polygon, Triangle *trianglesOut) {
    int tIdx = 0;
    if (polygon->numVertices < 3) {
        return 0;
    }

    for (int i = 2; i < polygon->numVertices; i++) {
        trianglesOut[tIdx].points[0] =  (Vec4f){ polygon->vertices[0].x, polygon->vertices[0].y, polygon->vertices[0].z, 1};
        trianglesOut[tIdx].points[1] =  (Vec4f){ polygon->vertices[i -1].x, polygon->vertices[i -1].y, polygon->vertices[i -1].z, 1};
        trianglesOut[tIdx].points[2] =  (Vec4f){ polygon->vertices[i].x, polygon->vertices[i].y, polygon->vertices[i].z, 1};
        trianglesOut[tIdx].uvTexture[0] = polygon->uvTextures[0];
        trianglesOut[tIdx].uvTexture[1] = polygon->uvTextures[i - 1];
        trianglesOut[tIdx].uvTexture[2] = polygon->uvTextures[i];
        tIdx++;
    }
    return tIdx;
}