#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef enum {
    POINTS = '1',
    OUTLINE = '2',
    FILL = '3',
    TEXTURE = '4',
    FILLED_OUTLINE = '5',
    TEXTURE_OUTLINE = '6',
    POINTS_OUTLINE = '7',
} RenderMode;

// An array of all the render options
static const RenderMode renderOptions[] = {
    POINTS,
    OUTLINE,
    FILL,
    TEXTURE,
    FILLED_OUTLINE,
    TEXTURE_OUTLINE,
    POINTS_OUTLINE,
};

static const int numRenderOptions = sizeof(renderOptions) / sizeof(renderOptions[0]);

typedef enum  {
    FORWARD = 'w',
    BACKWARD = 's',
    LEFT = 'a',
    RIGHT = 'd',
    UP = 'q',
    DOWN = 'e'
} MovementDirection;

// An array of all the movement directions
static const MovementDirection movementDirections[] = {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

static const int numMovementDirections = sizeof(movementDirections) / sizeof(movementDirections[0]);

typedef enum  {
    BACK_FACE_CULLING = '8',
    BACK_FACE_CULLING_DISABLED = '9'
} CullingMode;


bool isRenderOption(SDL_KeyCode key);
bool isMovementDirection(SDL_KeyCode key);

#endif /* INPUT_H */