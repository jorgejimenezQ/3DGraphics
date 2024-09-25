#ifndef MESH_H
#define MESH_H

#include "../texture/texture.h"
#include "geometry.h"
#include "matrix.h"
#include <stdio.h>


#define MAX_MATERIALS 30
/*********************************************************/
/*                                                      */
/*********************************************************/
typedef struct {
    Face* faces;
    Matrix vertices;
    Vec3f rotation;
    Vec3f scale;
    Vec3f translation;
    int numVertices;
    int numFaces;
} Mesh;

typedef struct {
    float r;
    float g;
    float b;
} DiffuseColor;

typedef struct {
    Matrix vertices;
    Face* faces;
    int numVertices;
    int numFaces;
    bool externalTexture;
    char* texturePath[MAX_MATERIALS];
    DiffuseColor colors[MAX_MATERIALS];
    Texture* textures[MAX_MATERIALS];
    int numMaterials;
} ModelData;   

typedef struct {
    Vec4f points[3];
    Vec2f uvTexture[3];
    uint32_t color;
    float avgDepth;
    ModelData *modelData;
} Triangle;


ModelData modelDataEmpty();

void loadCubeMeshData(void);
#endif
