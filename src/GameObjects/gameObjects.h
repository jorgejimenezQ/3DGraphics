#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

// Include any necessary libraries here
#include "../geometry/mesh.h"

// Define any constants or macros here
typedef struct {
    Vec3f position;                     // x, y, z
    ModelData* modelData;               // Mesh data
    Transformations transformations;    // Rotation, scale, translation
    int textureIndex;                   // Index of the texture in the texture array
} GameObject;

// Declare any structs or typedefs here

// Declare function prototypes here
GameObject createGameObject(ModelData *modelData, int textureIndex, Vec3f position, Vec3f rotation, Vec3f scale);
void destroyGameObject(GameObject* gameObject);

#endif // GAMEOBJECTS_H