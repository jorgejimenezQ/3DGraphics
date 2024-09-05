#include "./draw.h"
#include <stdio.h>
#include <stdlib.h>

int drawTriangle(Vec2f* points, uint32_t color, Display display) {
    drawLine(points[0], points[1], color, display);
    drawLine(points[1], points[2], color, display);
    drawLine(points[2], points[0], color, display);

    return 1;
}

int drawTriangleFill(Vec4f* points, uint32_t color, Display display) { // Sort the vertices
    Vec4f v1 = points[0];
    Vec4f v2 = points[1];
    Vec4f v3 = points[2];

    // zBuffer
    float* zBuffer = display.zBuffer;
    int bufferWidth = display.WINDOW_W;

    if (v1.y > v2.y) {
        swapVec4(&v1, &v2);
    }
    if (v1.y > v3.y) {
        swapVec4(&v1, &v3);
    }
    if (v2.y > v3.y) {
        swapVec4(&v2, &v3);
    }

    int xStart = v1.x;
    int xEnd = v1.x;
    if (v2.x < xStart) xStart = v2.x;
    if (v3.x < xStart) xStart = v3.x;
    if (v1.x < xStart) xStart = v1.x;

    if (v2.x > xEnd) xEnd = v2.x;
    if (v3.x > xEnd) xEnd = v3.x;
    if (v1.x > xEnd) xEnd = v1.x;

    Vec3f uvw;
    for (int y = v1.y; y <= v3.y; y++) {
        for (int x = xStart; x <= xEnd; x++) {
            Vec2f p = {x, y};
            if (!barycentric((Vec2f){v1.x, v1.y}, (Vec2f){v2.x, v2.y}, (Vec2f){v3.x, v3.y}, p, &uvw)) continue;
            if (uvw.x < 0 || uvw.y < 0 || uvw.z < 0) continue;
            if (uvw.x > 1 || uvw.y > 1 || uvw.z > 1) continue;
            if (v1.w == 0 || v2.w == 0 || v3.w == 0) continue;
            
            float w = (1/v1.w)*uvw.x + (1/v2.w)*uvw.y + (1/v3.w)*uvw.z;
            if (w == 0) continue;

            // Adjust 1/w so the pixels that are closer to the camera have smaller values
            w = 1.0 - w;

            // Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
            if (w > zBuffer[(bufferWidth*y) + x]) continue;
            drawPixel(x, y, color, display);
            // Upadate the zBuffer 
            zBuffer[(bufferWidth * y) + x] = w;
        }
    }
    return 1;
}