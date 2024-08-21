#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>

/*********************************************/
/*               GLOBAL VARIABLES            */
/*********************************************/

#define FPS 15
#define FRAME_TARGET_TIME (1000 / FPS)

typedef struct options {
    int windowWidth;
    int windowHeight;
    int renderMode;
} Options;

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* colorBufferTexture;
extern uint32_t resetColor; 
extern uint32_t* colorBuffer; // Buffer to store the color of each pixel in the window
extern float* zBuffer;
extern int WINDOW_W; // Window dimensions
extern int WINDOW_H;
extern bool fullScreen;

bool initWindow(int windowWidth, int windowHeight);
bool initFullscreenWindow(void);
void destroyWindow(void);
void renderColorBuffer(void);
void clearColorBuffer(uint32_t color);
void clearZBuffer(void);


#endif // DISPLAY_H