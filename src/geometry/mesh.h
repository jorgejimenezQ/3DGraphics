#ifndef MESH_H
#define MESH_H

#include "geometry.h"

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2) // 6 cube faces, 2 triangles per face

extern Vec3f cubeVertices[N_CUBE_VERTICES];
extern Face cubeFaces[N_CUBE_FACES];

/*********************************************************/
/*                                                      */
/*********************************************************/
typedef struct {
    Vec3f* vertices;
    Vec3f* normals;
    Face* faces;
    Vec3f rotation;
    Vec3f scale;
    Vec3f translation;
} Mesh;

extern Mesh mesh;

void loadCubeMeshData(void);
int loadObjFile(char* filepath);
#endif
