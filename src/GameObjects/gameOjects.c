#include "./gameObjects.h"

GameObject createGameObject(ModelData *modelData, int textureIndex, Vec3f position, Vec3f rotation, Vec3f scale) {
    GameObject gameObject;
    gameObject.position = position;
    gameObject.modelData = modelData;
    gameObject.transformations = (Transformations){.rotation = rotation, .scale = scale};

    gameObject.textureIndex = textureIndex;
    return gameObject;
}

void destroyGameObject(GameObject* gameObject) {
    free(gameObject);
}