#include <stdio.h>
#include <stdlib.h>
#include "mesh.h"
#include "../array.h"

Mesh mesh = {
    .vertices = NULL,
    .faces = NULL,
    .normals = NULL,
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

Face cubeFaces[N_CUBE_FACES] = {
    // front
    { .a = 1, .b = 2, .c = 3, .color = 0XFFFF0000},
    { .a = 1, .b = 3, .c = 4, .color = 0XFFFF0000},
    // right
    { .a = 4, .b = 3, .c = 5, .color = 0XFF00FF00},
    { .a = 4, .b = 5, .c = 6, .color = 0XFF00FF00},
    // back
    { .a = 6, .b = 5, .c = 7, .color = 0XFF0000FF},
    { .a = 6, .b = 7, .c = 8, .color = 0XFF0000FF},
    // left
    { .a = 8, .b = 7, .c = 2, .color = 0XFFFFFF00},
    { .a = 8, .b = 2, .c = 1, .color = 0XFFFFFF00},
    // top
    { .a = 2, .b = 7, .c = 5, .color = 0XFFAC239F},
    { .a = 2, .b = 5, .c = 3, .color = 0XFFAC239F},
    // bottom
    { .a = 6, .b = 8, .c = 1, .color = 0XFF234AC2},
    { .a = 6, .b = 1, .c = 4, .color = 0XFF234AC2}
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

//f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
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

        // if(ch != '#' ) hashCount = 0;
        // else if(ch == '#') hashCount++;
        // if (hashCount == 3) break;

        if (ch == 'f') {
            //f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
            int a = 0;
            int b = 0;
            int c = 0;
            // Textures
            int ta = 0;
            int tb = 0;
            int tc = 0;
            // Normals
            int na = 0;
            int nb = 0;
            int nc = 0;

            fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d", &a, &ta, &na, &b, &tb, &nb, &c, &tc, &nc);
            trashLine(file);
            Face face = {
                .a = a,
                .b = b,
                .c = c,
                .ta = ta,
                .tb = tb,
                .tc = tc,
                .na = na,
                .nb = nb,
                .nc = nc
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
        } else if (ch == 'v' && ch2 == 'n') {
            float x = 0.0;
            float y = 0.0;
            float z = 0.0;

            fscanf(file, " %f %f %f", &x, &y, &z);
            trashLine(file);
            Vec3f vertex = {
                .x = x,
                .y = y,
                .z = z
            };

            array_push(mesh.normals, vertex);
            // TODO: parse vn
        } else if (ch == 'v' && ch2 == 't') {
            float x = 0.0;
            float y = 0.0;

            fscanf(file, " %f %f", &x, &y);
            trashLine(file);
            Vec2f vertex = {
                .x = x,
                .y = y
            };

            array_push(mesh.textures, vertex);
        }   
    }
    fclose(file);
    return 0;
}   

