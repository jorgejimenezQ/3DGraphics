#ifndef TEXTURE_H
#define TEXTURE_H

// Include any necessary headers here
#include <stdint.h>

// Declare any necessary structs here
extern uint32_t* mesh_texture;
extern uint8_t REDBRICK_TEXTURE[];
extern uint32_t* brickTexture;
extern int brickTextureWidth;
extern int brickTextureHeight;
uint32_t applyLightIntensity(uint32_t color, float percentFactor);

// Declare any function prototypes
uint32_t* load_texture(const char* filename, int* width, int* height);


#endif // TEXTURE_H