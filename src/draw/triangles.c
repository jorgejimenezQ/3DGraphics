#include "./draw.h"
#include <stdio.h>
#include <stdlib.h>

int drawTriangle(Vec2f* points, uint32_t color, uint32_t* buffer, int bufferWidth, int boundX, int boundY) {
    drawLine(points[0], points[1], color, buffer, bufferWidth, boundX, boundY);
    drawLine(points[1], points[2], color, buffer, bufferWidth, boundX, boundY);
    drawLine(points[2], points[0], color, buffer, bufferWidth, boundX, boundY);

    return 1;
}

void drawTopTriangle(Vec2f v1, Vec2f v2, Vec2f m, uint32_t color, uint32_t* buffer, int bufferWidth, int boundX, int boundY){
    if (v2.x > m.x) {
        swap(&v2, &m);
    }   

    float dx = v1.x - v2.x;
    float dy = v1.y - v2.y;
    float mdx = v1.x - m.x;
    float mdy = v1.y - m.y;

    if (dy == 0) return;

    for (int y = v1.y; y <= v2.y; y++) {
        int startX = v1.x - (dx*v1.y - dx*y)/dy;
        int endX = v1.x - (mdx*v1.y - mdx*y)/mdy;
        for (int x = startX; x <= endX; x++) {
            drawPixel(x, y, color, buffer, bufferWidth, boundX, boundY);
        }
    }
}

void drawBottomTriangle(Vec2f v1, Vec2f v2, Vec2f m, uint32_t color, uint32_t* buffer, int bufferWidth, int boundX, int boundY){
    if (v2.x > m.x) swap(&v2, &m);

    float dx  = v1.x - v2.x;
    float dy  = v1.y - v2.y;
    float mdx = v1.x - m.x;
    float mdy = v1.y - m.y;

    if (dy == 0) return;

    for (int y = v1.y; y >= v2.y; y--) {
        int startX = v1.x - (dx*v1.y - dx*y)/dy;
        int endX = v1.x - (mdx*v1.y - mdx*y)/mdy;
        for (int x = startX; x <= endX; x++) {
            drawPixel(x, y, color, buffer, bufferWidth, boundX, boundY);
        }
    }
}


int drawTriangleFill(Vec2f* points, uint32_t color, uint32_t* buffer, int bufferWidth, int boundX, int boundY) {
    // Sort the vertices
    Vec2f v1 = points[0];
    Vec2f v2 = points[1];
    Vec2f v3 = points[2];

    if (v1.y > v2.y) swap(&v1, &v2);
    if (v1.y > v3.y) swap(&v1, &v3);
    if (v2.y > v3.y) swap(&v2, &v3);

    if (v1.y == v2.y) {
        drawBottomTriangle(v3, v1, v2, color, buffer, bufferWidth, boundX, boundY);
        return 1;
    }

    if (v2.y == v3.y) {
        drawTopTriangle(v1, v2, v3, color, buffer, bufferWidth, boundX, boundY);
        return 1;
    }

    // midpoint between v1 and v3 line
    Vec2f midpoint;

    // We know y
    midpoint.y = v2.y;
    float dx = v3.x - v1.x;
    float dy = v3.y - v1.y;

    if (dy == 0) return -1;
    midpoint.x = v1.x - (dx*v1.y - dx*v2.y)/dy;

    drawTopTriangle(v1, v2, midpoint, color, buffer, bufferWidth, boundX, boundY);
    drawBottomTriangle(v3, v2, midpoint, color, buffer, bufferWidth, boundX, boundY);

    return 1;
}