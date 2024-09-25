#ifndef RENDER_QUEUE_H
#define RENDER_QUEUE_H

// Include any necessary headers here
#include "../geometry/geometry.h"   

// Declare any global variables or constants here
#define MAX_RENDER_QUEUE_SIZE 10000

typedef struct {
    Triangle triangles[MAX_RENDER_QUEUE_SIZE];
    int front;
    int rear;
} RenderQueue;


// Declare any function prototypes here
void createRenderQueue(RenderQueue *renderQueue);
bool isRenderQueueEmpty(RenderQueue *renderQueue);
bool isRenderQueueFull(RenderQueue *renderQueue);
void enqueueRenderQueue(RenderQueue *renderQueue, Triangle triangle);
void dequeueRenderQueue(RenderQueue *renderQueue, Triangle *triangle);

#endif // RENDER_QUEUE_H