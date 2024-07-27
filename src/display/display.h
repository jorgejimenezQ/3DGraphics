#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>

/*********************************************/
/*               GLOBAL VARIABLES            */
/*********************************************/

#define FPS 20
#define FRAME_TARGET_TIME (1000 / FPS)

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* colorBufferTexture;
extern uint32_t resetColor; 
extern uint32_t* colorBuffer; // Buffer to store the color of each pixel in the window
extern int windowWidth; // Window dimensions
extern int windowHeight;

bool initWindow(void);
void destroyWindow(void);
void renderColorBuffer(void);
void clearColorBuffer(uint32_t color);

#endif // DISPLAY_H