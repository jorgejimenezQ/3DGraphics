#include "draw.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int drawPixel(int x, int y, uint32_t color, uint32_t* buffer, int bufferWidth, int boundX, int boundY) {

    // Keep the pixel bounded by boundX and boundY parameters and greater than one 
    if (x < 0 || y < 0) return -1;
    if (x > boundX || y > boundY) return -1;

    int idx = (y*bufferWidth) + x;

    // Check for out of array bounds
    if (!buffer[idx]) return -2;
    buffer[idx] = color;

    return 1;
}

int drawLine(Vec2f v1, Vec2f v2, uint32_t color, uint32_t *buffer, int bufferWith, int boundX, int boundY) {
    int dx = (v2.x - v1.x);   
    int dy = (v2.y - v1.y);   

    int sideLength = abs(dx) >= abs(dy) ? abs(dx) : abs(dy);

    float xInc = dx/(float) sideLength;
    float yInc = dy/(float) sideLength;

    float currentX = v1.x;
    float currentY = v1.y;

    for (int i = 0; i < sideLength; i++) {
        drawPixel(round(currentX), round(currentY), color, buffer, bufferWith, boundX, boundY);

        currentX += xInc;
        currentY += yInc;
    }

    return 1;
}  

int drawTriangle(Vec2f* points, uint32_t color, uint32_t* buffer, int bufferWidth, int boundX, int boundY) {
    drawLine(points[0], points[1], color, buffer, bufferWidth, boundX, boundY);
    drawLine(points[1], points[2], color, buffer, bufferWidth, boundX, boundY);
    drawLine(points[2], points[0], color, buffer, bufferWidth, boundX, boundY);

    return 1;
}