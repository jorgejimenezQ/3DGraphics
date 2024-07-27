#include <stdint.h>
#include "../draw/draw.h"


int rectangle(int x, int y, int width, int height, uint32_t color, uint32_t* buffer, int bufferWidth, int boundX, int boundY) {

    for (int i = 0; i < width; i++) 
        drawPixel(x+i, y, color, buffer, bufferWidth, boundX, boundY);

    for (int i = 0; i < height; i++) 
        drawPixel(x, y+i, color, buffer, bufferWidth, boundX, boundY);

    for (int i = 0; i < width; i++) 
        drawPixel(x+i, y+height, color, buffer, bufferWidth, boundX, boundY);

    for (int i = 0; i < height; i++) 
        drawPixel(x+width, y+i, color, buffer, bufferWidth, boundX, boundY);

    return 1;
}

int fillRect(int x, int y, int width, int height, uint32_t color, uint32_t* buffer, int bufferWidth, int boundX, int boundY) {
    for (int i = 0; i <= width; i++) {
        for (int j = 0; j <= height; j++) {
            // buffer[(y + j)*bufferWidth + (x + i)] = color;
            drawPixel(x+i, y+j, color, buffer, bufferWidth, boundX, boundY);
        }
    }

    return 1;
}
