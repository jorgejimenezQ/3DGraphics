#include <stdint.h>

int rectangle(int x, int y, int width, int height, uint32_t color, uint32_t* buffer, int bufferWidth) {
    for (int i = 0; i < width; i++) 
        buffer[(y*bufferWidth) + (x + i)] = color;

    for (int i = 0; i < height; i++) 
        buffer[((y + i)*bufferWidth) + x] = color;

    for (int i = 0; i < width; i++) 
        buffer[((y + height)*bufferWidth) + (x + i)] = color;

    for (int i = 0; i < height; i++) 
        buffer[((y + i)*bufferWidth) + (x + width)] = color;

    return 1;
}

int fillRect(int x, int y, int width, int height, uint32_t color, uint32_t* buffer, int bufferWidth) {
    for (int i = 0; i <= width; i++) {
        for (int j = 0; j <= height; j++) {
            buffer[(y + j)*bufferWidth + (x + i)] = color;
        }
    }

    return 1;
}
