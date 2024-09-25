#ifndef TEXTURE_H
#define TEXTURE_H

// Include any necessary headers here
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../utils/upng.h"

#define MAX_TEXTURES 10


// Declare any necessary structs here
typedef struct {
    uint32_t* data;
    unsigned int width;
    unsigned int height;
} Texture;


uint32_t applyLightIntensity(uint32_t color, float percentFactor);
Texture* loadTextureData(char* filepath);
void textureFree(Texture* texture);



// Declare any function prototypes
// uint32_t* load_texture(const char* filename, int* width, int* height);


#endif // TEXTURE_H