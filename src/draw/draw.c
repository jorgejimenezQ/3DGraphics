#include "draw.h"
#include <stdio.h>

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