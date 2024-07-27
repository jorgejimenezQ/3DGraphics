#ifndef DRAW_H
#define DRAW_H

#include <stdint.h>
#include "../geometry/geometry.h"

int drawPixel(int x, int y, uint32_t color, uint32_t* buffer, int bufferWidth, int boundX, int boundY);
int drawLine(Vec2f v1, Vec2f v2, uint32_t color, uint32_t *buffer, int bufferWith, int boundX, int boundY);
int drawTriangle(Vec2f* points, uint32_t color, uint32_t* buffer, int bufferWidth, int boundX, int boundY);


#endif // DRAW_H