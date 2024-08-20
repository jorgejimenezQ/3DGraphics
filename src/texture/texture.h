#ifndef TEXTURE_H
#define TEXTURE_H

// Include any necessary headers here
#include <stdint.h>

// Declare any necessary structs here
extern uint32_t* mesh_texture;
extern int meshTextureWidth;
extern int meshTextureHeight;
uint32_t applyLightIntensity(uint32_t color, float percentFactor);
int loadTextureFile(char* filepath, int* imageWidth, int* imageHeight);

// Declare any function prototypes
// uint32_t* load_texture(const char* filename, int* width, int* height);


#endif // TEXTURE_H