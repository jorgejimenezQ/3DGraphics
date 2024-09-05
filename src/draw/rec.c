#include "./draw.h"

int rectangle(int x, int y, int width, int height, uint32_t color, Display display) {

    for (int i = 0; i < width; i++) 
        drawPixel(x+i, y, color, display);

    for (int i = 0; i < height; i++) 
        drawPixel(x, y+i, color, display);

    for (int i = 0; i < width; i++) 
        drawPixel(x+i, y+height, color, display);

    for (int i = 0; i < height; i++) 
        drawPixel(x+width, y+i, color, display);

    return 1;
}

int fillRect(int x, int y, int width, int height, uint32_t color, Display display) {
    for (int i = 0; i <= width; i++) {
        for (int j = 0; j <= height; j++) {
            // buffer[(y + j)*bufferWidth + (x + i)] = color;
            drawPixel(x+i, y+j, color, display);
        }
    }

    return 1;
}