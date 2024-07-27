#ifndef DRAW_H
#define DRAW_H

#include <stdint.h>

int drawPixel(int x, int y, uint32_t color, uint32_t* buffer, int bufferWidth, int boundX, int boundY);

#endif // DRAW_H