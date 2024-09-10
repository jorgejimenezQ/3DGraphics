#include "display.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* colorBufferTexture = NULL;
// Buffer to store the color of each pixel in the window
static uint32_t* colorBuffer = NULL;
static float* zBuffer = NULL;

static uint32_t resetColor = 0XFF000000;
// Window dimensions
static int WINDOW_W = 320;
static int WINDOW_H = 240;
static bool isPixelated = true;
static int resolutionScale = 3;
// int windowWidth = 500;
// int windowHeight = 600;

static bool fullScreen = false;

WindowDimensions getWindowDimensions(void) {
    return (WindowDimensions){WINDOW_W, WINDOW_H};
}

uint32_t* getColorBuffer(void) {
    return colorBuffer;
}

void clearBuffer(uint32_t color) {
    renderColorBuffer(); // Render the color buffer to the screen
    // 0X00000000
    clearColorBuffer(color);
    clearZBuffer();

    SDL_RenderPresent(renderer);
}

void freeDisplay(void) {
    free(colorBuffer);
    free(zBuffer);
}

Display getDisplay(void) {
    return (Display){
        .colorBuffer = colorBuffer,
        .zBuffer = zBuffer,
        .window = window,
        .renderer = renderer,
        .colorBufferTexture = colorBufferTexture,
        .resetColor = resetColor,
        .WINDOW_W = WINDOW_W,
        .WINDOW_H = WINDOW_H,
        .fullScreen = fullScreen
    };
}

bool initFullScreenWindow() {
    fullScreen = true;

    // Get the current display mode of the primary display
    SDL_DisplayMode displayMode;
    int err = SDL_GetCurrentDisplayMode(0, &displayMode);
    if (err < 0) {
        // Print the error message
        fprintf(stderr, "Error getting the display mode: %s\n", SDL_GetError());
        exit(1);
    }
    // initWindow(displayMode.w, displayMode.h);
    return true;
}

bool initWindow(int windowWidth, int windowHeight, bool fullScreen) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    if (fullScreen) {
        // return true;
        // Get the current display mode of the primary display
        SDL_DisplayMode displayMode;
        int err = SDL_GetCurrentDisplayMode(0, &displayMode);
        if (err < 0) {
            // Print the error message
            fprintf(stderr, "Error getting the display mode: %s\n", SDL_GetError());
            exit(1);
        }
        fullScreen = true;
        windowWidth = displayMode.w;
        windowHeight = displayMode.h;
    }

    if (!isPixelated) {
        WINDOW_H = windowHeight;
        WINDOW_W = windowWidth;
    } else {
        WINDOW_H = windowHeight / resolutionScale;
        WINDOW_W = windowWidth / resolutionScale;
    }

    // Create a SDL window
    window = SDL_CreateWindow(
            "3D Graphics Engine", 
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
    if (fullScreen) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    // Allocate memory for the color buffer
    colorBuffer = (uint32_t*)malloc(sizeof(uint32_t) * WINDOW_W * WINDOW_H);
    if (!colorBuffer) {
        fprintf(stderr, "Error allocating memory for color buffer.\n");
        return false;
    }

    // Allocate memory for the z buffer
    zBuffer = (float*)malloc(sizeof(float) * WINDOW_W * WINDOW_H);

    if (!zBuffer) {
        fprintf(stderr, "Error allocating memory for z buffer.\n");
        return false;
    }

    // Create a SDL texture
    // SDL_PIXELFORMAT_ARGB8888 is a 32-bit pixel format, with alpha, red, green, and blue channels
    // check https://wiki.libsdl.org/SDL_CreateTexture for more information
    // SDL_TEXTUREACCESS_STREAMING means that the texture will be updated frequently
    colorBufferTexture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            WINDOW_W,
            WINDOW_H
    );

    return true;
}

void destroyWindow(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("Window destroyed.\n");
}

void clearZBuffer(void) {
    for (int i = 0; i < WINDOW_W * WINDOW_H; i++) {
        zBuffer[i] = 1.0;
    }
}

void clearColorBuffer(uint32_t color) {
    for (int i = 0; i < WINDOW_W * WINDOW_H; i++) {
        colorBuffer[i] = color;
    }
}

void renderColorBuffer(void) {
    // Update the SDL texture with new color buffer
    // check https://wiki.libsdl.org/SDL_UpdateTexture for more information
    SDL_UpdateTexture(
            colorBufferTexture, // The SDL texture to update
            NULL, //  A pointer to the rectangle of pixels to update, or NULL to update the entire texture
            colorBuffer, // The pixel data to update the texture with
            (int)(WINDOW_W * sizeof(uint32_t)) // The number of bytes in a row of pixel data, including padding between lines
    );

    // Copy the color buffer to the rendering target
    SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
}