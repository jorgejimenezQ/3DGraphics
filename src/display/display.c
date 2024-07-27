#include "display.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* colorBufferTexture = NULL;
// Buffer to store the color of each pixel in the window
uint32_t resetColor = 0XFF000000;
uint32_t* colorBuffer = NULL;

// Window dimensions
int windowWidth = 500;
int windowHeight = 600;

bool fullScreen = false;

bool initWindow(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // Get the current display mode of the primary display
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    // Full screen
    if (fullScreen) {
        windowWidth = displayMode.w;
        windowHeight = displayMode.h;
    }

    // Create a SDL window
    // window = SDL_CreateWindow(
    //         "3D Graphics Engine", 
    //         SDL_WINDOWPOS_CENTERED,
    //         SDL_WINDOWPOS_CENTERED,
    //         windowWidth,
    //         windowHeight,
    //         SDL_WINDOW_BORDERLESS       
    // );

    // Create a SDL window
    window = SDL_CreateWindow(
            "3D Graphics Engine", 
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            windowWidth,
            windowHeight,
            SDL_WINDOW_SHOWN       
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
    if (fullScreen) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    return true;
}

void destroyWindow(void) {
    free(colorBuffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("Window destroyed.\n");
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