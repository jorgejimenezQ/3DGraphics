#include <stdio.h>
#include <stdlib.h>
#include "mesh.h"
#include "../array.h"

Mesh mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = { 0, 0, 0 }
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

Face cubeFaces[N_CUBE_FACES] = {
    // front
    { .a = 1, .b = 2, .c = 3 },
    { .a = 1, .b = 3, .c = 4 },
    // right
    { .a = 4, .b = 3, .c = 5 },
    { .a = 4, .b = 5, .c = 6 },
    // back
    { .a = 6, .b = 5, .c = 7 },
    { .a = 6, .b = 7, .c = 8 },
    // left
    { .a = 8, .b = 7, .c = 2 },
    { .a = 8, .b = 2, .c = 1 },
    // top
    { .a = 2, .b = 7, .c = 5 },
    { .a = 2, .b = 5, .c = 3 },
    // bottom
    { .a = 6, .b = 8, .c = 1 },
    { .a = 6, .b = 1, .c = 4 }
};

void loadCubeMeshData(void) {
    for (int i = 0; i < N_CUBE_VERTICES; i++) {
        Vec3f cubeVertex = cubeVertices[i];
        array_push(mesh.vertices, cubeVertex);
    }
    for (int i = 0; i < N_CUBE_FACES; i++) {
        Face cubeFace = cubeFaces[i];
        array_push(mesh.faces, cubeFace);
    }
}

/******************************************************************/
/**                 R e a d i n g   F i l e                      **/
/******************************************************************/
void trashLine(FILE *file) {
    char ch;
    while((ch = fgetc(file)) != '\n');
}

int loadObjFile(char* filepath) {
    FILE *file;
    char ch;

    file = fopen(filepath, "r");
    if (file == NULL) {
        perror("Error opening the file");
        return 1;
    }
    while((ch = fgetc(file)) != EOF) {
        if (ch == '\n') continue;
        char ch2 =  fgetc(file);

        if (ch == 'f') {
            int a = 0;
            int b = 0;
            int c = 0;
            fscanf(file, "%d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &a, &b, &c);
            trashLine(file);
            Face face = {
                .a = a,
                .b = b,
                .c = c
            };
            array_push(mesh.faces, face);

        }  else if(ch == 'v' && ch2 == ' ') {
            float x = 0.0;
            float y = 0.0;
            float z = 0.0;

            fscanf(file, "%f %f %f", &x, &y, &z);
            trashLine(file);
            Vec3f vertex = {
                .x = x,
                .y = y,
                .z = z
            };

            array_push(mesh.vertices, vertex);
        } else {
            // TODO: parse vn and vt
            trashLine(file);
        }
    }

    fclose(file);

    return 0;
}   

