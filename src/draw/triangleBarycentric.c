#include "draw.h"
#include <pthread.h>

typedef struct {
    Vec2f* uvTexture;
    uint32_t* texture;
    int textureH;
    int textureW;
    Display display;
    int xStart;
    int xEnd;
    int yStart;
    int yEnd;
    Vec4f v1;
    Vec4f v2;
    Vec4f v3;
    Vec2f t1;
    Vec2f t2;
    Vec2f t3;
} TriangleThreadData;

// A thread function to draw a triangle
void* drawTriangleThread(void* arg) {
    TriangleThreadData* data = (TriangleThreadData*)arg;
    // drawTriangleBar(data->points, data->uvTexture, data->texture, data->textureH, data->textureW, data->display);
    float *zBuffer = data->display.zBuffer;
    Vec3f uvw;
    uint32_t color = 0xFFFFFFFF;
    int bufferWidth = data->display.WINDOW_W;
    Vec4f v1 = data->v1;
    Vec4f v2 = data->v2;
    Vec4f v3 = data->v3;
    Vec2f t1 = data->t1;
    Vec2f t2 = data->t2;
    Vec2f t3 = data->t3;
    Display display = data->display;
    int yStart = data->yStart;
    int yEnd = data->yEnd;
    uint32_t* texture = data->texture;
    int textureH = data->textureH;
    int textureW = data->textureW;

    for (int y = yStart; y <= yEnd; y++) {
        for (int x = data->xStart; x <= data->xEnd; x++) {
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
            drawPixel(x, y, color, display);
            // Upadate the zBuffer 
            zBuffer[(bufferWidth * y) + x] = w;
        }
    }
    return NULL;
}

// Draw a triangle using the barycentric coordinates

int drawTriangleBar(Vec4f* points, Vec2f* uvTexture, uint32_t* texture, int textureH, int textureW, Display display) {
    pthread_t threads[3];   
    TriangleThreadData threadData[3];
    
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

    // Calculate the area of the triangle using the threadData
    int scanLength = v3.y - v1.y;
    int linesPerThread = scanLength / 3;
    int remainingLines = scanLength % 3;

    for (int i = 0; i < 3; i++) {
        threadData[i].v1 = v1;
        threadData[i].v2 = v2;
        threadData[i].v3 = v3;
        threadData[i].t1 = t1;
        threadData[i].t2 = t2;
        threadData[i].t3 = t3;
        threadData[i].texture = texture;
        threadData[i].textureH = textureH;
        threadData[i].textureW = textureW;
        threadData[i].display = display;
        threadData[i].xStart = xStart;
        threadData[i].xEnd = xEnd;
        threadData[i].yStart = v1.y + (i * linesPerThread);
        threadData[i].yEnd = v1.y + ((i + 1) * linesPerThread) - 1;
        if (i == 2) {
            threadData[i].yEnd += remainingLines;
        }
        int rc = pthread_create(&threads[i], NULL, drawTriangleThread, &threadData[i]);

        if (rc) {
            printf("Error creating thread %d\n", i);
            exit(1);
        }
    }
    
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    

    return 1;
}