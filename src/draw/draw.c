#include "draw.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void swap(Vec2f* v1, Vec2f* v2) {
        Vec2f temp = *v2;
        *v2 = *v1;
        *v1 = temp;
}

void swapVec4(Vec4f* v1, Vec4f* v2) {
        Vec4f temp = *v2;
        *v2 = *v1;
        *v1 = temp;
}
int drawPixel(int x, int y, uint32_t color, Display display) {

    uint32_t* buffer = display.colorBuffer;
    int bufferWidth = display.WINDOW_W;
    int boundX = display.WINDOW_W;
    int boundY = display.WINDOW_H;
    

    // Keep the pixel bounded by boundX and boundY parameters and greater than one 
    // if (0 > x || x > (boundX - 1)) return -1;
    // if (0 > y || y > (boundY - 1)) return -1;
    if (x <= 0 || y <= 0) return -1;
    if (x >= boundX || y >= boundY) return -1;
    // if (x <= 0 || y <= 0) return -1;
    // if (x > boundX || y > boundY) return -1;


    int idx = (y*bufferWidth) + x;

    // Check for out of array bounds
    if (!buffer[idx]) return -2;
    buffer[idx] = color;

    return 1;
}

int drawLine(Vec2f v1, Vec2f v2, uint32_t color, Display display) {
    int dx = (v2.x - v1.x);   
    int dy = (v2.y - v1.y);   

    int sideLength = abs(dx) >= abs(dy) ? abs(dx) : abs(dy);

    float xInc = dx/(float) sideLength;
    float yInc = dy/(float) sideLength;

    float currentX = v1.x;
    float currentY = v1.y;

    for (int i = 0; i < sideLength; i++) {
        drawPixel(round(currentX), round(currentY), color, display);

        currentX += xInc;
        currentY += yInc;
    }

    return 1;
}  