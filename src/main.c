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
#include "./GameObjects/gameObjects.h"

/*
Actual FPS: 4
Render Mode: Textured
Back Face Culling: ON
Transformations: .
4231 triangles are being rendered
Mouse Position: (599, 699)
mouse change: (73896, 1349178568)

/////////////////////////////////////////////
time spent in each stage of the pipeline
Transformations      Clipping             Rendering            Total               
0.00116800           0.01378400           0.42410900           0.43906100          

Triangles per second Triangles per second Triangles per second Triangles per second
3622431.51           306950.09            9976.21              9636.47             

Time per triangle    Time per triangle    Time per triangle    Time per triangle   
0.00027606           0.00325786           0.10023848           0.10377239          

/////////////////////////////////////////////

*/

/************************************************************************ */
// Array of triangles to render on the screen
/************************************************************************ */
#define MAX_TRIANGLES_PER_MESH 10000
#define IS_DEBUG 1


// debug delete
int currentGameObject = 0;

const int MAP_WIDTH = 10;
const int MAP_HEIGHT = 10;
int map[MAP_HEIGHT][MAP_WIDTH] = {
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 2, 3, 3, 2, 3, 3, 2, 3, 3},
    {3, 2, 3, 3, 2, 3, 3, 2, 3, 3},
    {3, 2, 3, 2, 2, 2, 2, 2, 2, 3},
    {3, 2, 2, 2, 2, 2, 2, 2, 2, 3},
    {3, 2, 2, 2, 2, 2, 2, 2, 2, 3},
    {3, 2, 3, 2, 2, 3, 3, 2, 2, 3},
    {3, 2, 3, 2, 2, 3, 3, 2, 2, 3},
    {3, 2, 3, 2, 0, 3, 3, 2, 2, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
};

Vec3f cameraStartPosition = { .x = 0, .y = 0, .z = 0};

GameObject gameObjects[100];
int numGameObjects = 0;
ModelData cubeModelData;
ModelData floorModelData;
ModelData groundModelData;
ModelData alienModelData;

Texture* textures[MAX_TEXTURES];
int numTextures = 0;

bool CURSOR_ENABLED = false;

int brickTextureIndex;
int woodTextureIndex;
int startTextureIndex;
int groundTextureIndex;

int centerX;
int centerY;

/////////////////////////////////////////////////
///// TIMING VARIABLES
/////////////////////////////////////////////////
double TRANSFORMATIONS_TIME = 0.0;
double CLIPPING_TIME = 0.0;
double RENDERING_TIME = 0.0;

Triangle trianglesToRender[MAX_TRIANGLES_PER_MESH];
int numTrianglesToRender = 0;

Light light;

int actualFPS = 0;
Uint32 frameStart;
int frameTime;
// Color buffer
bool isRunning = false;
int prevFrameTime;

Texture theTexture; 

// array of three random colors
uint32_t BACKGROUND_COLOR = 0X000000FF;
uint32_t FOREGROUND_COLOR = 0XFFFFFFFF;
uint32_t OUTLINE_COLOR = 0XFF00FFFF;
uint32_t RED = 0XFF0000FF;
uint32_t GREEN = 0X00FF00FF;

// Render mode
RenderMode renderMode = FILL;
bool hasBackFaceCulling = true;
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
void renderTriangle(Triangle triangle, Display display, Texture* texture, DiffuseColor color);
void pipeLine(GameObject gameObject, float deltaTime);

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
}

void game(void) {
    clock_t start, end;
    double cpu_time_used;

    isRunning = initWindow(600, 600, false);
    // isRunning = initWindow(0, 0, true);
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
        numTrianglesToRender = 0;
        update(deltaTime / 1000.0);

        for (int i = 0; i < numGameObjects; i++) {
            currentGameObject = i;
            pipeLine(gameObjects[i], deltaTime / 1000.0);
        }
        rect(centerX, centerY, 2, 2, RED);

        /////////////////////////////////////////////////
        if (IS_DEBUG) start = clock();
        render();
        if (IS_DEBUG) {
            end = clock();
            cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
            RENDERING_TIME = cpu_time_used;
        }

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

            printf("The mouse cursor is %s\n", CURSOR_ENABLED ? "enabled" : "disabled");    
            printf("\n");

            if (IS_DEBUG) {
                printf("\n");
                printf("/////////////////////////////////////////////\n");
                // Column aligned output of the time taken for each stage with 2 decimal places
                int columnWidth = 20;
                printf("%-*s %-*s %-*s %-*s\n", columnWidth, "Transformations", columnWidth, "Clipping", columnWidth, "Rendering", columnWidth, "Total");
                // times
                printf("%-*.8f %-*.8f %-*.8f %-*.8f\n", columnWidth, TRANSFORMATIONS_TIME, columnWidth, CLIPPING_TIME, columnWidth, RENDERING_TIME, columnWidth, TRANSFORMATIONS_TIME + CLIPPING_TIME + RENDERING_TIME);
                printf("\n");
                // Triangles per second 
                printf("%-*s %-*s %-*s %-*s\n", columnWidth, "Triangles per second", columnWidth, "Triangles per second", columnWidth, "Triangles per second", columnWidth, "Triangles per second");
                printf("%-*.2f %-*.2f %-*.2f %-*.2f\n", columnWidth, numTrianglesToRender / TRANSFORMATIONS_TIME, columnWidth, numTrianglesToRender / CLIPPING_TIME, columnWidth, numTrianglesToRender / RENDERING_TIME, columnWidth, numTrianglesToRender / (TRANSFORMATIONS_TIME + CLIPPING_TIME + RENDERING_TIME));
                printf("\n");

                // Time per triangle in milliseconds
                printf("%-*s %-*s %-*s %-*s\n", columnWidth, "Time per triangle", columnWidth, "Time per triangle", columnWidth, "Time per triangle", columnWidth, "Time per triangle");
                printf("%-*.8f %-*.8f %-*.8f %-*.8f\n", columnWidth, TRANSFORMATIONS_TIME / numTrianglesToRender * 1000, columnWidth, CLIPPING_TIME / numTrianglesToRender * 1000, columnWidth, RENDERING_TIME / numTrianglesToRender * 1000, columnWidth, (TRANSFORMATIONS_TIME + CLIPPING_TIME + RENDERING_TIME) / numTrianglesToRender * 1000);
                printf("\n");

                printf("/////////////////////////////////////////////\n");
                // Display the model's current position
                // printf("Model Position: (%.2f, %.2f, %.2f)\n", model.transformations.translation.x, model.transformations.translation.y, model.transformations.translation.z);
                
            }
        }

        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_TARGET_TIME) {
            SDL_Delay(FRAME_TARGET_TIME - frameTime);
        }
    }

    freeResources();
    destroyWindow();
}

void pipeLine(GameObject gameObject, float deltaTime) {
    Display display = getDisplay();
    clock_t start, end;
    double cpu_time_used;

    /***************TRANSFORMATIONS MATRICES**************/
    gameObject.transformations.translation.x = gameObject.position.x;
    gameObject.transformations.translation.y = gameObject.position.y;
    gameObject.transformations.translation.z = gameObject.position.z;
    // gameObject.transformations.scale.x = 1;
    // gameObject.transformations.scale.y = 0;
    // gameObject.transformations.scale.z = 0;

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

    Transformations meshTransformations = gameObject.transformations;
    createScaleMatrix(&scaleMatrix, meshTransformations.scale.x, meshTransformations.scale.y, meshTransformations.scale.z);
    createTranslationMatrix(&translationMatrix, meshTransformations.translation.x, meshTransformations.translation.y, meshTransformations.translation.z);
    createRotationMatrix(&xRotationMatrix, meshTransformations.rotation.x, 'x');
    createRotationMatrix(&yRotationMatrix, meshTransformations.rotation.y, 'y');
    createRotationMatrix(&zRotationMatrix, meshTransformations.rotation.z, 'z');
    
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
    // (mouseDeltaX, mouseDeltaY, deltaTime);
    // updateCameraPosition(deltaTime);

    // calculate the target of the camera
    Vec3f target = vec3add(camera.position, camera.direction);
    createLookAt(camera.position, target, (Vec3f){0, 1, 0}, &viewMatrix);

    // /**********TRANSFORM THE WHOLE MATRIX OF COLUMN VECTORS************/
    /////////////////////////////////////////////////
    if (IS_DEBUG) start = clock();
    /////////////////////////////////////////////////

    Matrix transformedMatrix;
    transformedMatrix.data = NULL;  
    matrixMult(worldMatrix, gameObject.modelData->vertices, &transformedMatrix);
    matrixMult(viewMatrix, transformedMatrix, &transformedMatrix);

    /////////////////////////////////////////////////
    if (IS_DEBUG) {
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        TRANSFORMATIONS_TIME = cpu_time_used;
    }
    /////////////////////////////////////////////////

    /////////////////////////////////////////////////
    if (IS_DEBUG) start = clock();
    /////////////////////////////////////////////////
    // Get all the faces
    for (int i = 0; i < gameObject.modelData->numFaces && numTrianglesToRender < MAX_TRIANGLES_PER_MESH; i++) {
        Polygon polygon; // the polygon to be clipped
        Face currentFace = gameObject.modelData->faces[i]; // get the current face
        Matrix transformedVertices[3]; // the transformed vertices
        uint32_t faceColor = FOREGROUND_COLOR ; // the face color

        // transformFace(currentFace, &worldMatrix, &viewMatrix, transformedVertices);     


        /******************************************************/
        // BACK FACE CULLING
        // Perform back face culling if enabled and it's the third vertex of the triangle
        // Calculate the face normal
        // Check if the face is facing away from the camera
        Vec3f faceNormal; // We are saving the face normal for the lighting calculations
        // Copy the vertices to the transformed vertices from the meshes vertices matrix
        transformedVertices[0].cols = 1;
        transformedVertices[1].cols = 1;
        transformedVertices[2].cols = 1;
        transformedVertices[0].rows = 4;
        transformedVertices[1].rows = 4;
        transformedVertices[2].rows = 4;

        transformedVertices[0].data = matrixCol(transformedMatrix, currentFace.indices.x);
        transformedVertices[1].data = matrixCol(transformedMatrix, currentFace.indices.y);
        transformedVertices[2].data = matrixCol(transformedMatrix, currentFace.indices.z);

        if (hasBackFaceCulling && isBackface(transformedVertices, &faceNormal)) {
            for (int k = 0;k < 3; k++) {
                matrixFree(transformedVertices[k]);
            }
            continue;
        }
        
        DiffuseColor color = gameObject.modelData->colors[currentFace.materialIndex];
        if (renderMode == FILL || renderMode == TEXTURE || renderMode == FILLED_OUTLINE || renderMode == TEXTURE_OUTLINE) {
            float dotProd = -vec3dot(faceNormal, light.direction);
            color.r = color.r * fabs(dotProd);
            color.g = color.g * fabs(dotProd);
            color.b = color.b * fabs(dotProd);
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
            // Does the face have a texture
            Texture* texture = NULL;

            if (gameObject.modelData->texturePath[currentFace.materialIndex] != NULL) {
                texture = gameObject.modelData->textures[currentFace.materialIndex];
                renderTriangle(newTriangles[j], display, texture, color);
            } else {
                renderTriangle(newTriangles[j], display, NULL, color);
            }
            numTrianglesToRender++; // for debugging purposes and performance monitoring

            // Free the column vectors
            matrixFree(tempTriMatrix[0]);
            matrixFree(tempTriMatrix[1]);
            matrixFree(tempTriMatrix[2]);
        }

        for (int k = 0;k < 3; k++) {
            matrixFree(transformedVertices[k]);
        }
    } // end of for loop - faces

    /////////////////////////////////////////////////
    if (IS_DEBUG) {
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        CLIPPING_TIME = cpu_time_used;
    }
    /////////////////////////////////////////////////

    // Free the matrices
    matrixFree(worldMatrix);
    matrixFree(scaleMatrix);
    matrixFree(translationMatrix);
    matrixFree(xRotationMatrix);
    matrixFree(yRotationMatrix);
    matrixFree(zRotationMatrix);
    matrixFree(viewMatrix);
}

uint32_t createColor32(float r, float g, float b, float a) {
    uint32_t color = 0;
    color += (uint8_t)(r * 255) << 24;
    color += (uint8_t)(g * 255) << 16;
    color += (uint8_t)(b * 255) << 8;
    color += (uint8_t)(a * 255);
    return color;
}

void renderTriangle(Triangle triangle, Display display, Texture* texture, DiffuseColor color) {
    uint32_t color32int;
    color32int = createColor32(color.r, color.g, color.b, 1.0);

    Vec2f tPoints[] = {
        (Vec2f){triangle.points[0].x, triangle.points[0].y},
        (Vec2f){triangle.points[1].x, triangle.points[1].y},
        (Vec2f){triangle.points[2].x, triangle.points[2].y}
    };
    if (renderMode == FILL || renderMode == FILLED_OUTLINE) {
        drawTriangleFill(triangle.points, color32int, display);
    }
    
    if (texture != NULL && (renderMode == TEXTURE || renderMode == TEXTURE_OUTLINE)) {
        drawTriangleBar(triangle.points, triangle.uvTexture, texture->data, texture->height, texture->width, display);
        // drawOptimizedTriangle(triangle, display, mesh_texture, meshTextureHeight, meshTextureWidth);
    }

    if (renderMode == OUTLINE || renderMode == FILLED_OUTLINE || renderMode == TEXTURE_OUTLINE || renderMode == POINTS_OUTLINE) {
        drawTriangle(tPoints, OUTLINE_COLOR, display);
    }

    if (renderMode == POINTS || renderMode == POINTS_OUTLINE) {
        rect(triangle.points[0].x, triangle.points[0].y, 2, 2, GREEN);
        rect(triangle.points[1].x, triangle.points[1].y, 2, 2, GREEN);
        rect(triangle.points[2].x, triangle.points[2].y, 2, 2, GREEN);
    }
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

GameObject createCube(ModelData *modelData, int textureIndex, Vec3f position) {
    GameObject gameObject;
    gameObject.position = position;
    gameObject.modelData = modelData;
    gameObject.transformations = (Transformations){.rotation = (Vec3f){0, 0, 0}, .scale = (Vec3f){1, 1, 1}};

    gameObject.textureIndex = textureIndex;
    return gameObject;
}

void generateMap(){
        // -0 is starting point
        // -1 is no render
        // -2 is floor
        // -3 is a cube wall
        // -4 is flat wall
    // use the map array to generate the game objects 
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int tile = map[y][x];

            Vec3f position = { .x = x * 2, .y = 0, .z = y * 2};
            ModelData *modelData = NULL;
            int textureIndex = 0;
            switch(tile) {
                case 0: {
                    modelData = &floorModelData;
                    textureIndex = startTextureIndex;

                    // Save the start position
                    camera.position = position;
                    camera.position.y = 2;
                    break;
                }
                case 2: {
                    modelData = &floorModelData;
                    textureIndex = woodTextureIndex;
                    break;
                }
                case 3: {
                    position.y = 2;
                    modelData = &cubeModelData;
                    textureIndex = brickTextureIndex;
                    break;
                }
            }

            GameObject gameObject = createCube(modelData, textureIndex, position);
            gameObjects[numGameObjects] = gameObject;
            numGameObjects++;
        }
    }
    
}

void setup(void) {

    centerX = getDisplay().WINDOW_W / 2;
    centerY = getDisplay().WINDOW_H / 2;

    // Get the window dimensions
    WindowDimensions windowDimensions = getWindowDimensions();

    // Light
    light.direction = vec3normalize((Vec3f){0, 0, 1});

    // cubeModelData = loadMeshData("assets/new assets/cube.obj");
    // floorModelData = loadMeshData("assets/floor.obj");
    // groundModelData = loadMeshData("assets/space models/groundTile.obj");


    // textures[numTextures] = loadTextureData("assets/default_acacia_wood.png");
    // woodTextureIndex = numTextures;
    // numTextures++;

    // textures[numTextures] = loadTextureData("assets/default_brick.png");
    // brickTextureIndex = numTextures;
    // numTextures++;

    // textures[numTextures] = loadTextureData("assets/FyAccGonfalon.png");
    // startTextureIndex = numTextures;
    // numTextures++;

    // create the game object for the ground
    // gameObjects[numGameObjects++] = createCube(&groundModelData, -1, (Vec3f){0, 0, 5});
    // generateMap();

    alienModelData = loadMeshData("assets/racing/car-parsche-sport-violet.obj");
    gameObjects[numGameObjects++] = createCube(&alienModelData, -1, (Vec3f){0, 0, 5});

    // cubeModelData = loadMeshData("assets/Dungeon Set 2/struct_block_normal.obj");
    // gameObjects[numGameObjects++] = createCube(&cubeModelData, -1, (Vec3f){0, 0, 10});

    // floorModelData = loadMeshData("assets/Dungeon Set 2/struct_floor_normal.obj");
    // gameObjects[numGameObjects++] = createCube(&floorModelData, -1, (Vec3f){0, 0, 15});

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
                // mouseDeltaX = mouseX - event.motion.x;
                // mouseDeltaY = mouseY - event.motion.y;
                // mouseX = event.motion.x;
                // mouseY = event.motion.y;

                 // Use relative motion from the event (not the absolute position)
                mouseDeltaX = event.motion.xrel * -1; // Relative x movement
                mouseDeltaY = event.motion.yrel * -1; // Relative y movement


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
    // Update the texture with new color buffer
    // renderColorBuffer(); // Render the color buffer to the screen
    // // 0X00000000
    // clearColorBuffer(BACKGROUND_COLOR);
    // clearZBuffer();
    clearBuffer(BACKGROUND_COLOR);
}
void freeResources() {
    for (int i = 0; i < numTextures; i++) {
        textureFree(textures[i]);
    }

    array_free(cubeModelData.faces);
    array_free(floorModelData.faces);
    freeDisplay();
    matrixFree(perspectiveMatrix);

    matrixFree(cubeModelData.vertices);
    matrixFree(floorModelData.vertices);
}

void updateCameraPosition(float dt) {
    int speed = 10;
    switch(keyPressed) {
        case UP: {
            camera.position.y += speed * dt;
            break;
        }
        case DOWN: {
            camera.position.y -= speed * dt;
            break;
        }
        case LEFT: {
            camera.position = vec3add(camera.position, vec3mult(vec3cross(camera.direction, (Vec3f){0, 1, 0}), speed * dt));
           
            break;
        }
        case RIGHT: {
            camera.position = vec3sub(camera.position, vec3mult(vec3cross(camera.direction, (Vec3f){0, 1, 0}), speed * dt));
            break;
        }
        case FORWARD: {
            Vec3f forwardProjection = { .x = camera.direction.x, .y = 0, .z = camera.direction.z};
            Vec3f velocity = vec3mult(forwardProjection, speed * dt);
            camera.position = vec3add(camera.position, velocity);
            break;
        }
        case BACKWARD: {
            Vec3f forwardProjection = { .x = camera.direction.x, .y = 0, .z = camera.direction.z};
            Vec3f velocity = vec3mult(forwardProjection, speed * dt);
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
    float sensitivity = 1.0;
    camera.yaw += deltaX * sensitivity * dt;
    camera.pitch += deltaY * sensitivity * dt;

    if (camera.pitch > 89.0) camera.pitch = 89.0;
    if (camera.pitch < -89.0) camera.pitch = -89.0;

    camera.direction.x = cos(deg2rad(camera.yaw)) * cos(deg2rad(camera.pitch));
    camera.direction.y = sin(deg2rad(camera.pitch));
    camera.direction.z = sin(deg2rad(camera.yaw)) * cos(deg2rad(camera.pitch));
    camera.direction = vec3normalize(camera.direction);
}