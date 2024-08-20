#include <stdio.h>
#include <stdint.h>
#include "../utils/upng.h"

uint32_t* mesh_texture = NULL;
int meshTextureWidth = 0;
int meshTextureHeight = 0;

uint32_t applyLightIntensity(uint32_t color, float percentFactor) {
    uint32_t maskA = 0XFF000000;
    uint32_t maskR = 0X00FF0000;
    uint32_t maskG = 0X0000FF00;
    uint32_t maskB = 0X000000FF;


    uint32_t a = (color & maskA);
    uint32_t r = (color & maskR) * percentFactor;
    uint32_t g = (color & maskG) * percentFactor;
    uint32_t b = (color & maskB) * percentFactor;

    return a | (r & maskR) | (g & maskG) | (b & maskB);
}


int loadTextureFile(char* filepath, int* imageWidth, int* imageHeight) {
    upng_t* upng;

    upng = upng_new_from_file(filepath);
    if (upng == NULL) return -1;

    upng_decode(upng);
    if (upng_get_error(upng) == UPNG_EOK) {
        *imageWidth = upng_get_width(upng);
        *imageHeight = upng_get_height(upng);
        mesh_texture = (uint32_t*)upng_get_buffer(upng);
    } else {
        printf("Error loading texture: %s\n", filepath);
        return -1;
    }
    
    
    upng_free(upng);
    return 1;
}