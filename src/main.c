#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "errors/errors.h"


SDL_Window* window;
SDL_Renderer* renderer;
bool isRunning = false;

int windowWidth = 800;
int windowHeight = 600;

// COLORS
u_int32_t resetColor = 0XFF000000;

uint32_t* colorBuffer = NULL;
SDL_Texture* colorBufferTexture = NULL;

bool initWindow(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // Get the current display mode of the primary display
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    windowWidth = displayMode.w;
    windowHeight = displayMode.h;


    // Create a SDL window
    window = SDL_CreateWindow(
            NULL, 
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            windowWidth,
            windowHeight,
            SDL_WINDOW_BORDERLESS       
    );

    if (!window) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // TODO:  Create a SDL renderer 
    renderer = SDL_CreateRenderer(
            window, 
                -1,
                0
    );     
    if (!renderer) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // Set the window to fullscreen
    // SDL_WINDOW_FULLSCREEN_DESKTOP is used to set the window to fullscreen and
    // keep the same resolution as the desktop. SDL_WINDOW_FULLSCREEN can be used to set the window to fullscreen with the resolution of the window
    // check https://wiki.libsdl.org/SDL_SetWindowFullscreen for more information
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    return true;
}

void destroyWindow(void) {
    free(colorBuffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("Window destroyed.\n");
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

void drawGrid(void) {
    for (int x = 0; x < windowWidth; x++) {
        for (int y = 0; y < windowHeight; y++) {
                if (x % 10 == 0 || y % 10 == 0) {
                    int idx = (windowWidth * y) + x;
                    colorBuffer[idx] = 0xffffffff;
                }
        }
    }
}

void update(void) {
    drawGrid();
}


void clearColorBuffer(uint32_t color) {
    for (int y = 0; y < windowHeight; y++) {
        for (int x = 0; x < windowWidth; x++) {
            colorBuffer[(windowWidth * y) + x] = color;
        }
    }
}

void renderColorBuffer(void) {
    // Update the SDL texture with new color buffer
    // check https://wiki.libsdl.org/SDL_UpdateTexture for more information
    SDL_UpdateTexture(
            colorBufferTexture, // The SDL texture to update
            NULL, //  A pointer to the rectangle of pixels to update, or NULL to update the entire texture
            colorBuffer, // The pixel data to update the texture with
            (int)(windowWidth * sizeof(uint32_t)) // The number of bytes in a row of pixel data, including padding between lines
    );

    // Copy the color buffer to the rendering target
    SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
}

void render(void) {
    // Set the color to clear the window with
    // check https://wiki.libsdl.org/SDL_SetRenderDrawColor for more information
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
    SDL_RenderClear(renderer); // Clear the back buffer

    // Update the texture with new color buffer
    renderColorBuffer(); // Render the color buffer to the screen
    // 0X00000000
    clearColorBuffer(resetColor);

    SDL_RenderPresent(renderer);
}

int main(void) {
    isRunning = initWindow();

    setup();

    while(isRunning) {
        processInput();
        update();
        render();
    }

    destroyWindow();
    return 0;
}