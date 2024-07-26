#include "./display/display.h"
#include "errors/errors.h"
#include "./excercises/rectangles.c"

bool isRunning = false;



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

void update(void) {
    rectangle(100, 100, 200, 100,0XFF0000FF, colorBuffer, windowWidth);
    fillRect(200, 300, 200, 100,0XFFFF00FF, colorBuffer, windowWidth);
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