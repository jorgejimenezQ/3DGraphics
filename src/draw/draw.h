#ifndef DRAW_H
#define DRAW_H

#include <stdlib.h>
#include <stdint.h>
#include "../display/display.h"
#include "../geometry/geometry.h"

typedef struct {
    int bufferWidth;
    uint32_t* buffer;
    int boundX;
    int boundY;
} DrawingContext;

// PIXELS & LINES
int drawPixel(int x, int y, uint32_t color, Display display);
int drawLine(Vec2f v1, Vec2f v2, uint32_t color, Display display);
// TRIANGLES
int drawTriangle(Vec2f* points, uint32_t color, Display display);
int drawTriangleFill(Vec4f* points, uint32_t color, Display display);
int drawTriangleBar(Vec4f* points, Vec2f* uvTexture, uint32_t* texture, int textureH, int textureW, Display display);

int drawOptimizedTriangle(Vec4f* points, uint32_t color, Display display);

// RECTANGLES
int fillRect(int x, int y, int width, int height, uint32_t color, Display display);
int rectangle(int x, int y, int width, int height, uint32_t color, Display display);

// HELPERS
void swap(Vec2f* v1, Vec2f* v2);
void swapVec4(Vec4f* v1, Vec4f* v2);

#endif // DRAW_H