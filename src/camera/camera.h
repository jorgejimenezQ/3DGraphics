#ifndef CAMERA_H
#define CAMERA_H

// Include any necessary libraries or headers
#include "../geometry/geometry.h"
#include "../geometry/matrix.h"

// Declare any structs or types
typedef struct {
    Vec3f position;
    Vec3f direction;
    Vec3f forwardVelocity;
    float yaw;
    float pitch;
} Camera;

// Define any constants or macros
extern Camera camera;


// Declare function prototypes

#endif /* CAMERA_H */