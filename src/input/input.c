#include "input.h"

bool isRenderOption(SDL_KeyCode key) {
    // Compare the key to the render options enum
    for (int i = 0; i < numRenderOptions; i++) {
        if ((enum RenderMode)key == renderOptions[i]) {
            return true;
        }
    }

    return false;
}

bool isMovementDirection(SDL_KeyCode key) {
    // Compare the key to the movement directions enum
    for (int i = 0; i < numMovementDirections; i++) {
        if ((enum MovementDirection)key == movementDirections[i]) {
            return true;
        }
    }

    return false;
}