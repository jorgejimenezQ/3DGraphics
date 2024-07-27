#include "./display/display.h"
#include "errors/errors.h"
#include "./excercises/rectangles.c"
#include "./draw/draw.h"
#include "./geometry/geometry.h"
#include "./geometry/mesh.h"

Triangle trianglesToRender[N_MESH_FACES];

const int numPoints = 9 * 9 * 9;
Vec3f cubePoints[numPoints];
Vec3f cubeRotation = { .x = 0, .y = 0, .z = 0 };

int actualFPS = 0;
Uint32 frameStart;
int frameTime;

Vec2f projectedPoints[numPoints];

float fovFactor = 620;
bool isRunning = false;
int prevFrameTime;
Vec3f camPosition = { .x = 0, .y = 0, .z = -5};

// array of three random colors
uint32_t colorsRand[3] = {0XFFFFFFFF, 0XFFFF00FF, 0XFF00FFF0};

void rect(int x, int y, int w, int h, uint32_t color) {
    fillRect(
            x, 
            y,
            w,
            h,
            color,
            colorBuffer,
            windowWidth,
            windowWidth -1,
            windowHeight - 1
    );

}

void setup(void) {
    // (uint32_t*) is a type cast to convert the pointer to a uint32_t pointer
    // malloc is a function that allocates memory in the heap
    colorBuffer = (uint32_t*)malloc(sizeof(uint32_t) * windowWidth * windowHeight);
    if (!colorBuffer) {
        fprintf(stderr, "Error allocating memory for color buffer.\n");
        exit(1);
    }

    // Create a SDL texture
    // SDL_PIXELFORMAT_ARGB8888 is a 32-bit pixel format, with alpha, red, green, and blue channels
    // check https://wiki.libsdl.org/SDL_CreateTexture for more information
    // SDL_TEXTUREACCESS_STREAMING means that the texture will be updated frequently
    colorBufferTexture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            windowWidth,
            windowHeight
    );

}

void processInput(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type) {
        case SDL_QUIT: {
            isRunning = false;
            break;
        }
        case SDL_KEYDOWN: {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                isRunning = false;
            }
        }
        default: {
            break;
        }
    }
}

/*****************************************************/
// Takes a 3D vector and returns a projected 2d point
Vec2f project(Vec3f point ) {
    Vec2f projectedPoint = {
        .x = (fovFactor * point.x) / point.z,
        .y = (fovFactor * point.y) /point.z
    };
    
    return projectedPoint;
}

void update(void) {
    // int timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - prevFrameTime);
    // // printf("Time to wait: %d\n", timeToWait);
    // if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME) {
    //     actualFPS = 0;
    //     SDL_Delay(timeToWait);
    // }

    prevFrameTime = SDL_GetTicks();

    cubeRotation.x += 0.01;
    cubeRotation.y += 0.01;
    cubeRotation.z += 0.01;

    // Get all the faces
    for (int i = 0; i < N_MESH_FACES; i++) {
        Face meshFace = meshFaces[i];

        Vec3f faceVertices[3] = {
            meshVertices[meshFace.a - 1], 
            meshVertices[meshFace.b - 1], 
            meshVertices[meshFace.c - 1] 
        };
        Triangle projectedTriangle;

        for (int j = 0; j < 3; j++) {
            Vec3f vertex = faceVertices[j];

            // Transformations
            Vec3f transformedVertex = rotateX(vertex, cubeRotation.x);
            transformedVertex = rotateY(transformedVertex, cubeRotation.y);
            transformedVertex = rotateZ(transformedVertex, cubeRotation.z);
            transformedVertex.z -= camPosition.z;

            // Translate  the vertex away fro the camera
            transformedVertex.z -= camPosition.z;

            // Project the current vertex
            Vec2f projectedVertex = project(transformedVertex);

            // Scale and translate to the middle of the screen
            projectedVertex.x += (windowWidth/2);
            projectedVertex.y += (windowHeight/2);

            // Save the projected triangle
            projectedTriangle.points[j] = projectedVertex;
        }
        // our array of triangles
        trianglesToRender[i] = projectedTriangle;
    }
}

void renderLine(Vec2f v1, Vec2f v2, uint32_t color) {
    drawLine(v1, v2, color, colorBuffer, windowWidth, windowWidth, windowHeight);
}


void render(void) {
    for (int i = 0; i < N_MESH_FACES; i++) {
        Triangle triangle = trianglesToRender[i];
        rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0XFFFFFF00);
        rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0XFFFFFF00);
        rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0XFFFFFF00);
        drawTriangle(triangle.points, colorsRand[i % 3], colorBuffer, windowWidth, windowWidth, windowHeight);
    }

    // Update the texture with new color buffer
    renderColorBuffer(); // Render the color buffer to the screen

    // 0X00000000
    clearColorBuffer(resetColor);

    SDL_RenderPresent(renderer);
}

int main(void) {
    isRunning = initWindow();

    setup();

    Uint32 startTime = SDL_GetTicks();
    int frameCount = 0;

    while(isRunning) {
        frameStart = SDL_GetTicks();

        processInput();
        update();
        render();

        frameCount++;
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - startTime >= 1000) {
            actualFPS = frameCount;
            frameCount = 0;
            startTime = currentTime;
            // Clear the terminal screen before printing the actual FPS
            system("clear");
            printf("Actual FPS: %d\n", actualFPS);
        }

        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_TARGET_TIME) {
            SDL_Delay(FRAME_TARGET_TIME - frameTime);
        }
    }

    destroyWindow();
    return 0;
}