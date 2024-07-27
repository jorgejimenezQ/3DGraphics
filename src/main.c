#include "./display/display.h"
#include "errors/errors.h"
#include "./excercises/rectangles.c"
#include "./draw/draw.h"
#include "./geometry/geometry.h"

const int numPoints = 9 * 9 * 9;
Vec3f cubePoints[numPoints];
Vec2f projectedPoints[numPoints];

float fovFactor = 700;
bool isRunning = false;

Vec3f camPosition = { .x = 0, .y = 0, .z = -5};

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

    int pointCount = 0;
    for (float x = -1; x <= 1; x += 0.25) {
        for (float y = -1; y <= 1; y += 0.25) {
            for (float z = -1; z <= 1; z += 0.25) {
                Vec3f point = { .x = x, .y = y, .z = z };
                cubePoints[pointCount++] = point;
            }
        }
    }
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
        .y = (fovFactor * point.y) / point.z
    };
    
    return projectedPoint;
}

void update(void) {
    for (int i = 0; i < numPoints; i++) {
        Vec3f point = cubePoints[i];
        point.z -= camPosition.z;
        // project the point
        Vec2f projectedPoint = project(point);
        projectedPoints[i] = projectedPoint;
    }

}



void render(void) {
    for (int i = 0; i < numPoints; i++) {
        Vec2f projectedPoint = projectedPoints[i];
        fillRect(
                projectedPoint.x + windowWidth/2, 
                projectedPoint.y + windowHeight/2, 
                4, 4,
                0XFFFFFF00,
                colorBuffer,
                windowWidth,
                windowWidth -1,
                windowHeight - 1
        );
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

    // in mega bytes
    // 1 byte = 8 bits
    // 1 kilobyte = 1024 bytes
    // 1 megabyte = 1024 kilobytes = 1024 * 1024 bytes
    // printf("An array of 1,000,000,000 Vec4f is %zu gigabytes\n", a * 1000000000 / (1024 * 1024 * 1024));

    while(isRunning) {
        processInput();
        update();
        render();
    }

    destroyWindow();
    return 0;
}