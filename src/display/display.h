#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/*********************************************/
/*               GLOBAL VARIABLES            */
/*********************************************/

#define FPS 24
#define FRAME_TARGET_TIME (1000 / FPS)

typedef struct {
    int windowWidth;
    int windowHeight;
    int renderMode;
} Options;

typedef struct {
    int width;
    int height;
} WindowDimensions;

typedef struct {
    uint32_t* colorBuffer;
    float* zBuffer;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* colorBufferTexture;
    uint32_t resetColor;
    int WINDOW_W;
    int WINDOW_H;
    bool fullScreen;
} Display;

bool initWindow(int windowWidth, int windowHeight, bool fullScreen);
bool initFullScreenWindow(void);
void destroyWindow(void);
void renderColorBuffer(void);
void clearColorBuffer(uint32_t color);
void clearZBuffer(void);
WindowDimensions getWindowDimensions(void);
Display getDisplay(void);
uint32_t* getColorBuffer(void);
void clearBuffer(uint32_t color);
void freeDisplay(void);

#endif // DISPLAY_H