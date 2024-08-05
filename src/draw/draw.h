#ifndef DRAW_H
#define DRAW_H

#include <stdint.h>
#include "../geometry/geometry.h"

// PIXELS & LINES
int drawPixel(int x, int y, uint32_t color, uint32_t* buffer, int bufferWidth, int boundX, int boundY);
int drawLine(Vec2f v1, Vec2f v2, uint32_t color, uint32_t *buffer, int bufferWith, int boundX, int boundY);

// TRIANGLES
int drawTriangle(Vec2f* points, uint32_t color, uint32_t* buffer, int bufferWidth, int boundX, int boundY);
int drawTriangleFill(Vec2f* points, uint32_t color, uint32_t* buffer, int bufferWidth, int boundX, int boundY);

// RECTANGLES
int fillRect(int x, int y, int width, int height, uint32_t color, uint32_t* buffer, int bufferWidth, int boundX, int boundY);
int rectangle(int x, int y, int width, int height, uint32_t color, uint32_t* buffer, int bufferWidth, int boundX, int boundY);

// HELPERS
void swap(Vec2f* v1, Vec2f* v2);
#endif // DRAW_H