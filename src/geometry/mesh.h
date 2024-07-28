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
    Face* faces;
    Vec3f rotation;
} Mesh;

extern Mesh mesh;

void loadCubeMeshData(void);

#endif
