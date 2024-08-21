#include "./texture.h"
#include "../utils/upng.h"

uint32_t* mesh_texture = NULL;
int meshTextureWidth = 0;
int meshTextureHeight = 0;

uint32_t applyLightIntensity(uint32_t color, float percentFactor) {
    uint32_t maskR = 0XFF000000;
    uint32_t maskG = 0X00FF0000;
    uint32_t maskB = 0X0000FF00;
    uint32_t maskA = 0X000000FF;


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
        *imageWidth = (int)upng_get_width(upng);
        *imageHeight = (int)upng_get_height(upng);
        
        // Copy the texture data to the mesh_texture array so we can free the upng object
        // and still have access to the texture data
        unsigned int size = upng_get_size(upng) / sizeof(uint32_t);
        mesh_texture = (uint32_t*)malloc(size * sizeof(uint32_t));
        const unsigned char *data = upng_get_buffer(upng);
        for (int i = 0; i < (int)size; i++) {
            mesh_texture[i] = 0;
            mesh_texture[i] =
                ((uint32_t)data[i * 4 + 0] << 24) |  // Red
                ((uint32_t)data[i * 4 + 1] << 16) |  // Blue -> Second byte
                ((uint32_t)data[i * 4 + 2] << 8)  |  // Green -> Third byte
                ((uint32_t)data[i * 4 + 3] << 0);    // Alpha -> Fourth byte
        }

    } else {
        printf("Error loading texture: %s\n", filepath);
        return -1;
    }
    
    upng_free(upng);
    return 1;
}

void textureFree() {
    free(mesh_texture);
    mesh_texture = NULL;
    meshTextureWidth = 0;
    meshTextureHeight = 0;
}