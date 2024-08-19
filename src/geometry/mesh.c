#include <stdio.h>
#include <stdlib.h>
#include "mesh.h"
#include "../array.h"

Mesh mesh = {
    // .vertices = NULL,
    .faces = NULL,
    // .normals = NULL,
    .rotation = { 0, 0, 0 },
    .scale = { 1, 1, 1},
    .translation = { 0, 0, 0}
};

Vec3f cubeVertices[N_CUBE_VERTICES] = {
    { .x = -1, .y = -1, .z = -1 }, // 1
    { .x = -1, .y =  1, .z = -1 }, // 2
    { .x =  1, .y =  1, .z = -1 }, // 3
    { .x =  1, .y = -1, .z = -1 }, // 4
    { .x =  1, .y =  1, .z =  1 }, // 5
    { .x =  1, .y = -1, .z =  1 }, // 6
    { .x = -1, .y =  1, .z =  1 }, // 7
    { .x = -1, .y = -1, .z =  1 }  // 8
};


/******************************************************************/
/**                 R e a d i n g   F i l e                      **/
/******************************************************************/
void trashLine(FILE *file) {
    char ch;
    while((ch = fgetc(file)) != '\n');
}