#include "input/input.h"
#include "array.h"
#include "./display/display.h"
#include "errors/errors.h"
#include "./draw/draw.h"
#include "./geometry/geometry.h"
#include "./geometry/mesh.h"
#include "./utils/utils.h"
#include "./geometry/matrix.h"
#include "./geometry/transformations.h"
#include "./texture/texture.h"
#include "./utils/utils.h"
#include "./camera/camera.h"
#include "./clipping/clipping.h"

/************************************************************************ */
// Array of triangles to render on the screen
/************************************************************************ */
#define MAX_TRIANGLES_PER_MESH 10000

Triangle trianglesToRender[MAX_TRIANGLES_PER_MESH];
int numTrianglesToRender = 0;

// Light
Light light;

int actualFPS = 0;
Uint32 frameStart;
int frameTime;
// Color buffer
bool isRunning = false;
int prevFrameTime;

// array of three random colors
uint32_t BACKGROUND_COLOR = 0X333333FF;
uint32_t FOREGROUND_COLOR = 0XFFFFFFFF;
uint32_t OUTLINE_COLOR = 0XFF00FFFF;
uint32_t RED = 0XFF0000FF;
uint32_t GREEN = 0X00FF00FF;

// Render mode
RenderMode renderMode = POINTS_OUTLINE;
bool hasBackFaceCulling = false;
char transformations = '.';
Matrix perspectiveMatrix;

// mouse
int mouseX = 0;
int mouseY = 0;
float mouseDeltaX = 0;
float mouseDeltaY = 0;

// Keyboard
MovementDirection keyPressed = '.';

// drawing context
DrawingContext dc;

Vec3f pointToRender[20];

float dt = 0.0;

void rect(int x, int y, int w, int h, uint32_t color);
void setup(void);
void processInput(void);
void update(float deltaTime);
void render(void);
void game(void);
void freeResources(void);
void updateCameraPosition(float dt);
void updateCameraDirection(float deltaX, float deltaY, float dt);

int main(void) {
    game();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Process the graphics pipeline stages for all the mesh triangles
///////////////////////////////////////////////////////////////////////////////
// +-------------+
// | Model space |  <-- original mesh vertices
// +-------------+
// |   +-------------+
// `-> | World space |  <-- multiply by world matrix
//     +-------------+
//     |   +--------------+
//     `-> | Camera space |  <-- multiply by view matrix
//         +--------------+
//         |    +------------+
//         `--> |  Clipping  |  <-- clip against the six frustum planes
//              +------------+
//              |    +------------+
//              `--> | Projection |  <-- multiply by projection matrix
//                   +------------+
//                   |    +-------------+
//                   `--> | Image space |  <-- apply perspective divide
//                        +-------------+
//                        |    +--------------+
//                        `--> | Screen space |  <-- ready to render
//                             +--------------+
///////////////////////////////////////////////////////////////////////////////
void update(float deltaTime) {
    int numFaces = array_length(mesh.faces);

    /***************TRANSFORMATIONS MATRICES**************/
    // mesh.translation.z = 6;

    Matrix scaleMatrix;
    Matrix translationMatrix;
    Matrix xRotationMatrix;
    Matrix yRotationMatrix;
    Matrix zRotationMatrix;

    matrixCreate(4, 4, &scaleMatrix);
    matrixCreate(4, 4, &translationMatrix);
    matrixCreate(4, 4, &xRotationMatrix);
    matrixCreate(4, 4, &yRotationMatrix);
    matrixCreate(4, 4, &zRotationMatrix);

    createScaleMatrix(&scaleMatrix, mesh.scale.x, mesh.scale.y, mesh.scale.z);
    createTranslationMatrix(&translationMatrix, mesh.translation.x, mesh.translation.y, mesh.translation.z);
    createRotationMatrix(&xRotationMatrix, mesh.rotation.x, 'x');
    createRotationMatrix(&yRotationMatrix, mesh.rotation.y, 'y');
    createRotationMatrix(&zRotationMatrix, mesh.rotation.z, 'z');
    
    /***************WORLD MATRICES**************/
    Matrix worldMatrix;
    matrixCreate(4, 4, &worldMatrix);

    matrixIdentity(4, 4, &worldMatrix);
    matrixMult(xRotationMatrix, worldMatrix, &worldMatrix);
    matrixMult(yRotationMatrix, worldMatrix, &worldMatrix);
    matrixMult(zRotationMatrix, worldMatrix, &worldMatrix);
    matrixMult(scaleMatrix, worldMatrix, &worldMatrix);
    matrixMult(translationMatrix, worldMatrix, &worldMatrix);

    /**********VIEW MATRIX************/
    Matrix viewMatrix;
    matrixCreate(4, 4, &viewMatrix);
    // updateCameraDirection(mouseDeltaX, mouseDeltaY, deltaTime);
    // updateCameraPosition(deltaTime);

    // calculate the target of the camera
    Vec3f target = vec3add(camera.position, camera.direction);
    createLookAt(camera.position, target, (Vec3f){0, 1, 0}, &viewMatrix);

    numTrianglesToRender = 0;
    // Get all the faces
    for (int i = 0; i < numFaces && numTrianglesToRender < MAX_TRIANGLES_PER_MESH; i++) {
        Polygon polygon; // the polygon to be clipped
        Face currentFace = mesh.faces[i]; // get the current face
        Matrix transformedVertices[3]; // the transformed vertices
        uint32_t faceColor = FOREGROUND_COLOR ; // the face color

        transformFace(currentFace, &worldMatrix, &viewMatrix, transformedVertices);     
        
        /******************************************************/
        // BACK FACE CULLING
        // Perform back face culling if enabled and it's the third vertex of the triangle
        // Calculate the face normal
        // Check if the face is facing away from the camera
        Vec3f faceNormal; // We are saving the face normal for the lighting calculations
        if (hasBackFaceCulling && isBackface(transformedVertices, &faceNormal)) {
            for (int k = 0;k < 3; k++) {
                matrixFree(transformedVertices[k]);
            }
            continue;
        }
        
        if (renderMode == FILL || renderMode == TEXTURE || renderMode == FILLED_OUTLINE || renderMode == TEXTURE_OUTLINE) {
            float dotProd = -vec3dot(faceNormal, light.direction);
            faceColor = applyLightIntensity(faceColor, fabs(dotProd));
        }

        /*******************C L I P P I N G**********************************/
        //************************************************************/
        Vec3f vertex0 = createVec3data(transformedVertices[0].data);
        Vec3f vertex1 = createVec3data(transformedVertices[1].data);
        Vec3f vertex2 = createVec3data(transformedVertices[2].data);
        polygon = createPolygonFromTriangle(vertex0, vertex1, vertex2, 
                currentFace.uvTexture[0], currentFace.uvTexture[1], currentFace.uvTexture[2]);

        // Clip the polygon
        clipPolygon(&polygon);

        // A column vector that will temporarily hold the vector column formed
        // by the vertices of the triangles
        if (polygon.numVertices < 3) {
            for (int k = 0;k < 3; k++) {
                matrixFree(transformedVertices[k]);
            }
            continue;
        }

        Matrix tempTriMatrix[3];
        Triangle newTriangles[polygon.numVertices - 2];
        int numTriangles = polygonToTriangles(&polygon, newTriangles);

        /******************************************************/
        // PROJECT EACH TRIANGLE RETURNED FROM THE CLIPPING
        for (int j = 0; j < numTriangles; j++) {
            // project the triangle
            matrixCreateFromV4(newTriangles[j].points[0], &tempTriMatrix[0]);
            matrixCreateFromV4(newTriangles[j].points[1], &tempTriMatrix[1]);
            matrixCreateFromV4(newTriangles[j].points[2], &tempTriMatrix[2]);
            WindowDimensions windowDimensions = getWindowDimensions();

            // our array of triangles
            projectTriangle(tempTriMatrix, &perspectiveMatrix, windowDimensions.width, windowDimensions.height, &newTriangles[j]);
            newTriangles[j].color = faceColor;

            trianglesToRender[numTrianglesToRender] = newTriangles[j];
            numTrianglesToRender++;

            // Free the column vectors
            matrixFree(tempTriMatrix[0]);
            matrixFree(tempTriMatrix[1]);
            matrixFree(tempTriMatrix[2]);
        }

        for (int k = 0;k < 3; k++) {
            matrixFree(transformedVertices[k]);
        }
    } // end of for loop - faces

    // Free the matrices
    matrixFree(worldMatrix);
    matrixFree(scaleMatrix);
    matrixFree(translationMatrix);
    matrixFree(xRotationMatrix);
    matrixFree(yRotationMatrix);
    matrixFree(zRotationMatrix);
    matrixFree(viewMatrix);
}

void game(void) {
    // isRunning = initWindow(600, 700, false);
    isRunning = initWindow(0, 0, true);
    setup();

    Uint32 startTime = SDL_GetTicks();
    int frameCount = 0;
    Uint32 previousTime = startTime;
    Uint32 deltaTime = 0; 

    while(isRunning) {
        frameStart = SDL_GetTicks();

        processInput();
        // Update the game state with the time difference in seconds
        dt = deltaTime / 1000.0;
        update(deltaTime / 1000.0);
        render();

        frameCount++;
        Uint32 currentTime = SDL_GetTicks();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        // Get the number of cores in the CPU
        // printf("Number of cores: %d\n", SDL_GetCPUCount());

        if (currentTime - startTime >= 1000) {
            actualFPS = frameCount;
            frameCount = 0;
            startTime = currentTime;
            // Clear the terminal screen before printing the actual FPS
            system("clear");
            // Color red if under 24 FPS
            // else color green
            if (actualFPS < FPS) printf("\033[0;31m");
            else printf("\033[0;32m");

            printf("Actual FPS: %d\n", actualFPS);
            // reset the color to default
            printf("\033[0m");
            printf("Render Mode: %c\n", renderMode);
            printf("Back Face Culling: %s\n", hasBackFaceCulling ? "ON" : "OFF");
            printf("Transformations: %c\n", transformations);
            printf("%d triangles are being rendered\n", numTrianglesToRender);

            // Pint the mouse position
            printf("Mouse Position: (%d, %d)\n", mouseX, mouseY);
            printf("mouse change: (%d, %d)\n", mouseDeltaX, mouseDeltaY);
            // reset the color to default
            printf("\033[0m");
        }

        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_TARGET_TIME) {
            SDL_Delay(FRAME_TARGET_TIME - frameTime);
        }
    }

    freeResources();
    destroyWindow();
}

void rect(int x, int y, int w, int h, uint32_t color) {
    Display display = getDisplay();
    fillRect(
            x, 
            y,
            w,
            h,
            color,
            display
    );

}

void setup(void) {
    // Light
    light.direction = vec3normalize((Vec3f){0, 0, 1});
    // loadCubeMeshData();
    if (loadObjFile("assets/new assets/f117.obj", &mesh) == -1) {
        printf("Error loading the mesh file\n");
        exit(1);
    }

    if (loadTextureFile("assets/new assets/f117.png", &meshTextureWidth, &meshTextureHeight) == -1) {
        printf("Error loading the png file\n");
        exit(1);
    }

    WindowDimensions windowDimensions = getWindowDimensions();
    // Initialize the perspective projection matrix
    float aspectY = (float)windowDimensions.height/(float)windowDimensions.width;
    float aspectX = (float)windowDimensions.width/(float)windowDimensions.height;
    float fovY =  3.141592 / 3.0; // the same as 180/3, or 60deg
    float fovX = atan(tan(fovY/2) * aspectX) * 2;
    float znear = 1;
    float zfar = 20.0;
    createPerspectiveMatrix(&perspectiveMatrix, fovY, aspectY, znear, zfar);

    // Initialize the frustum
    initializeFrustumPlanes(fovX, fovY, znear, zfar);
}

void processInput(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT: {
                isRunning = false;
                break;
            }
            case SDL_MOUSEMOTION: {
                mouseDeltaX = mouseX - event.motion.x;
                mouseDeltaY = mouseY - event.motion.y;
                mouseX = event.motion.x;
                mouseY = event.motion.y;

                updateCameraDirection(mouseDeltaX, mouseDeltaY, dt);
                break;
            }
            case SDL_KEYDOWN: {
                SDL_KeyCode key = event.key.keysym.sym;
                if (key == SDLK_ESCAPE) {
                    isRunning = false;
                    break;
                }

                if (isRenderOption(key)) {
                    renderMode = (RenderMode)key;
                    break; 
                }


                if (key == BACK_FACE_CULLING || key == BACK_FACE_CULLING_DISABLED) {
                    hasBackFaceCulling = (key == BACK_FACE_CULLING) ? true : false;
                    break;
                }

                if (isMovementDirection(key)) {
                    keyPressed = (MovementDirection)key;
                    updateCameraPosition(dt);
                    break;
                }
                break;
            }
            default: {
                break;
            }
        }

    }
}

void renderLine(Vec2f v1, Vec2f v2, uint32_t color) {
    Display display = getDisplay();
    drawLine(v1, v2, color, display);
}

void render(void) {
    Display display = getDisplay();
    // for (int i = numTrianglesToRender - 1; i >= 0; i--) {
    for (int i = 0; i < numTrianglesToRender; i++) {
        Triangle triangle = trianglesToRender[i];
        Vec2f tPoints[] = {
            (Vec2f){triangle.points[0].x, triangle.points[0].y},
            (Vec2f){triangle.points[1].x, triangle.points[1].y},
            (Vec2f){triangle.points[2].x, triangle.points[2].y}
        };
        if (renderMode == FILL || renderMode == FILLED_OUTLINE) {
            drawTriangleFill(triangle.points, triangle.color, display);
        }
        
        if (renderMode == TEXTURE || renderMode == TEXTURE_OUTLINE) {
            drawTriangleBar(triangle.points, triangle.uvTexture, mesh_texture, meshTextureHeight, meshTextureWidth, display);
        }

        if (renderMode == OUTLINE || renderMode == FILLED_OUTLINE || renderMode == TEXTURE_OUTLINE || renderMode == POINTS_OUTLINE) {
            drawTriangle(tPoints, OUTLINE_COLOR, display);
        }

        if (renderMode == POINTS || renderMode == POINTS_OUTLINE) {
            rect(triangle.points[0].x, triangle.points[0].y, 5, 5, GREEN);
            rect(triangle.points[1].x, triangle.points[1].y, 5, 5, GREEN);
            rect(triangle.points[2].x, triangle.points[2].y, 5, 5, GREEN);
        }
    }

    // Update the texture with new color buffer
    // renderColorBuffer(); // Render the color buffer to the screen
    // // 0X00000000
    // clearColorBuffer(BACKGROUND_COLOR);
    // clearZBuffer();
    clearBuffer(BACKGROUND_COLOR);

}
void freeResources() {
    textureFree();
    freeDisplay();
    matrixFree(perspectiveMatrix);
    array_free(mesh.faces);
}

void updateCameraPosition(float dt) {
    switch(keyPressed) {
        case UP: {
            camera.position.y += 3 * dt;
            break;
        }
        case DOWN: {
            camera.position.y -= 3 * dt;
            break;
        }
        case LEFT: {
            camera.position = vec3sub(camera.position, vec3mult(vec3cross(camera.direction, (Vec3f){0, 1, 0}), 3 * dt));
           
            break;
        }
        case RIGHT: {
            camera.position = vec3add(camera.position, vec3mult(vec3cross(camera.direction, (Vec3f){0, 1, 0}), 3 * dt));
            break;
        }
        case FORWARD: {
            Vec3f forwardProjection = { .x = camera.direction.x, .y = 0, .z = camera.direction.z};
            Vec3f velocity = vec3mult(forwardProjection, 3 * dt);
            camera.position = vec3add(camera.position, velocity);
            break;
        }
        case BACKWARD: {
            Vec3f forwardProjection = { .x = camera.direction.x, .y = 0, .z = camera.direction.z};
            Vec3f velocity = vec3mult(forwardProjection, 3 * dt);
            camera.position = vec3sub(camera.position, velocity);

            break;
        }
    }
    keyPressed = '.';
}
 
float deg2rad(float degrees) {
    return degrees * (3.14159265359 / 180.0);
}

void updateCameraDirection(float deltaX, float deltaY, float dt) {
    if (deltaX == 0 && deltaY == 0) return;
    float sensitivity = 3;
    camera.yaw += deltaX * sensitivity * dt;
    camera.pitch += deltaY * sensitivity * dt;

    if (camera.pitch > 89.0) camera.pitch = 89.0;
    if (camera.pitch < -89.0) camera.pitch = -89.0;

    camera.direction.x = cos(deg2rad(camera.yaw)) * cos(deg2rad(camera.pitch));
    camera.direction.y = sin(deg2rad(camera.pitch));
    camera.direction.z = sin(deg2rad(camera.yaw)) * cos(deg2rad(camera.pitch));
    camera.direction = vec3normalize(camera.direction);
}