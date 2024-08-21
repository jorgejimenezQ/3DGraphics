#include "draw.h"


int drawTriangleBar(Vec4f* points, Vec2f* uvTexture, uint32_t* texture, int textureH, int textureW, uint32_t* buffer, int bufferWidth, int boundX, int boundY) {

    // Sort the vertices
    Vec4f v1 = points[0];
    Vec4f v2 = points[1];
    Vec4f v3 = points[2];
    // Texture points
    Vec2f t1 = uvTexture[0];
    Vec2f t2 = uvTexture[1];
    Vec2f t3 = uvTexture[2];

    if (v1.y > v2.y) {
        swapVec4(&v1, &v2);
        swap(&t1, &t2);
    }
    if (v1.y > v3.y) {
        swapVec4(&v1, &v3);
        swap(&t1, &t3);
    }
    if (v2.y > v3.y) {
        swapVec4(&v2, &v3);
        swap(&t2, &t3);
    }

    int xStart = v1.x;
    int xEnd = v1.x;
    if (v2.x < xStart) xStart = v2.x;
    if (v3.x < xStart) xStart = v3.x;
    if (v1.x < xStart) xStart = v1.x;

    if (v2.x > xEnd) xEnd = v2.x;
    if (v3.x > xEnd) xEnd = v3.x;
    if (v1.x > xEnd) xEnd = v1.x;
    Vec3f uvw;
    uint32_t color = 0xFFFFFFFF;
    for (int y = v1.y; y <= v3.y; y++) {
        for (int x = xStart; x <= xEnd; x++) {
            Vec2f p = {x, y};
            if (!barycentric((Vec2f){v1.x, v1.y}, (Vec2f){v2.x, v2.y}, (Vec2f){v3.x, v3.y}, p, &uvw)) continue;
            if (uvw.x < 0 || uvw.y < 0 || uvw.z < 0) continue;
            if (uvw.x > 1 || uvw.y > 1 || uvw.z > 1) continue;
            if (v1.w == 0 || v2.w == 0 || v3.w == 0) continue;
            
            float u = (t1.x/v1.w)*uvw.x + (t2.x/v2.w)*uvw.y + (t3.x/v3.w)*uvw.z;
            float v = (t1.y/v1.w)*uvw.x + (t2.y/v2.w)*uvw.y + (t3.y/v3.w)*uvw.z;
            float w = (1/v1.w)*uvw.x + (1/v2.w)*uvw.y + (1/v3.w)*uvw.z;
            if (w == 0) continue;
            u /= w;
            v /= w;

            // Adjust 1/w so the pixels that are closer to the camera have smaller values
            w = 1.0 - w;

            // Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
            if (w > zBuffer[(bufferWidth*y) + x]) continue;

            int textureX = abs((int)(u * textureW));
            int textureY = (int)(v * textureH);
            color = texture[textureY*textureW + textureX];
            drawPixel(x, y, color, buffer, bufferWidth, boundX, boundY);
            // Upadate the zBuffer 
            zBuffer[(bufferWidth * y) + x] = w;
        }
    }
    return 1;
}