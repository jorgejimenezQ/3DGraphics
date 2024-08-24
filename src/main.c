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
#include "input/input.h"

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
uint32_t BACKGROUND_COLOR = 0X555555FF;
uint32_t FOREGROUND_COLOR = 0XFFFFFFFF;
uint32_t OUTLINE_COLOR = 0XFFFFFFFF;
uint32_t RED = 0XFF0000FF;
uint32_t GREEN = 0XFFFFFFFF;

// Render mode
RenderMode renderMode = OUTLINE;
bool hasBackFaceCulling = true;
char transformations = '.';
Matrix perspectiveMatrix;

// 
float dt = 0.0;

// drawing context
DrawingContext dc;

Vec3f pointToRender[20];

void rect(int x, int y, int w, int h, uint32_t color);
void setup(void);
void processInput(void);
void update(float deltaTime);
void render(void);
void game(void);
void processTransformation(void);
void freeResources(void);
void moveCamera(MovementDirection key);

int main(void) {
    game();
    // Matrix m;
    // // print the memory address of the matrix
    // printf("Memory address of the matrix: %p\n", &m);
    // // print the memory address of the matrix's data
    // printf("Memory address of the matrix's data: %p\n", m.data);

    // matrixCreate(4, 4, &m);
    // matrixIdentity(4, 4, &m);
    // Matrix b;
    // matrixCopy(m, &b);

    // // print the memory address of the matrix
    // printf("Memory address of the matrix: %p\n", &m);
    // // print the memory address of the matrix's data
    // printf("Memory address of the matrix's data: %p\n", m.data);

    // matrixPrint(m);
    // matrixFree(m);
    // matrixFree(b);
    return 0;
}

void update(float deltaTime) {

    int numFaces = array_length(mesh.faces);

    /***************TRANSFORMATIONS MATRICES**************/
    mesh.translation.z = 4;

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
    // TODO: Handle the camera movement. the target and the up vector
    Matrix temp;
    matrixCreateWithData(4, 1, (float[]){0, 0, 1, 1}, &temp);
    Matrix yawRotation;
    matrixCreate(4, 4, &yawRotation);
    createRotationMatrix(&yawRotation, camera.yaw, 'y');
    matrixMult(yawRotation, temp, &temp);  

    camera.direction.x = temp.data[0];
    camera.direction.y = temp.data[1];
    camera.direction.z = temp.data[2];

    Vec3f target = vec3add(camera.position, camera.direction);

    createLookAt(camera.position, 
            target,
            (Vec3f){0, 1, 0}, 
            &viewMatrix
    );

    matrixFree(temp);
    matrixFree(yawRotation);

    numTrianglesToRender = 0;
    // Get all the faces
    for (int i = 0; i < numFaces && numTrianglesToRender < MAX_TRIANGLES_PER_MESH; i++) {

        Polygon polygon; // the polygon to be clipped
        Face currentFace = mesh.faces[i]; // get the current face
        Triangle projectedTriangle; // the projected triangle
        Matrix transformedVertices[3]; // the transformed vertices
        uint32_t faceColor = FOREGROUND_COLOR ; // the face color

        transformFace(currentFace, &worldMatrix, &viewMatrix, transformedVertices);     
        
        /******************************************************/
        // BACK FACE CULLING
        // Perform back face culling if enabled and it's the third vertex of the triangle
        // Calculate the face normal
        // Check if the face is facing away from the camera
        Vec3f faceNormal; // We are saving the face normal for the lighting calculations
        if (hasBackFaceCulling && isBackface(transformedVertices, &faceNormal)) 
            break;
        
        if (renderMode == FILL || renderMode == TEXTURE) {
            float dotProd = -vec3dot(faceNormal, light.direction);
            faceColor = applyLightIntensity(faceColor, fabs(dotProd));
            projectedTriangle.color = faceColor;
        }

        /*******************C L I P P I N G**********************************/
        //************************************************************/
        Vec3f vertex0 = createVec3data(transformedVertices[0].data);
        Vec3f vertex1 = createVec3data(transformedVertices[1].data);
        Vec3f vertex2 = createVec3data(transformedVertices[2].data);
        polygon = createPolygonFromTriangle(vertex0, vertex1, vertex2);

        // Clip the polygon
        clipPolygon(&polygon);
        int numTriangles = polygon.numVertices - 2;

        // A column vector that will temporarily hold the vector column formed
        // by the vertices of the triangles
        Matrix tempTriMatrix[3];
        Triangle newTriangles[numTriangles];
        polygonToTriangles(&polygon, newTriangles);

        /******************************************************/
        // PROJECT EACH TRIANGLE RETURNED FROM THE CLIPPING
        for (int j = 0; j < numTriangles; j++) {
            // project the triangle
            matrixCreateFromV4(newTriangles[j].points[0], &tempTriMatrix[0]);
            matrixCreateFromV4(newTriangles[j].points[1], &tempTriMatrix[1]);
            matrixCreateFromV4(newTriangles[j].points[2], &tempTriMatrix[2]);

            projectTriangle(tempTriMatrix, &perspectiveMatrix, WINDOW_W, WINDOW_H, &projectedTriangle);
            trianglesToRender[numTrianglesToRender] = newTriangles[j];
            numTrianglesToRender++;

            // Free the column vectors
            matrixFree(tempTriMatrix[0]);
            matrixFree(tempTriMatrix[1]);
            matrixFree(tempTriMatrix[2]);

            // our array of triangles
            projectedTriangle.uvTexture[0] = currentFace.uvTexture[0];
            projectedTriangle.uvTexture[1] = currentFace.uvTexture[1];
            projectedTriangle.uvTexture[2] = currentFace.uvTexture[2];

            trianglesToRender[numTrianglesToRender] = projectedTriangle;
            numTrianglesToRender++;
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
    isRunning = initWindow(800, 700);

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
        update(dt);
        render();

        frameCount++;
        Uint32 currentTime = SDL_GetTicks();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        if (currentTime - startTime >= 1000) {
            actualFPS = frameCount;
            frameCount = 0;
            startTime = currentTime;
            // Clear the terminal screen before printing the actual FPS
            system("clear");
            // Color red if under 24 FPS
            // else color green
            if (actualFPS < 24) printf("\033[0;31m");
            else printf("\033[0;32m");

            printf("Actual FPS: %d\n", actualFPS);
            // reset the color to default
            printf("\033[0m");
            printf("Render Mode: %c\n", renderMode);
            printf("Back Face Culling: %s\n", hasBackFaceCulling ? "ON" : "OFF");
            printf("Transformations: %c\n", transformations);
            printf("%d triangles are being rendered\n", numTrianglesToRender);
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
    fillRect(
            x, 
            y,
            w,
            h,
            color,
            colorBuffer,
            WINDOW_W,
            WINDOW_W -1,
            WINDOW_H - 1
    );

}

void setup(void) {
    // (uint32_t*) is a type cast to convert the pointer to a uint32_t pointer
    // malloc is a function that allocates memory in the heap
    colorBuffer = (uint32_t*)malloc(sizeof(uint32_t) * WINDOW_W * WINDOW_H);
    if (!colorBuffer) {
        fprintf(stderr, "Error allocating memory for color buffer.\n");
        exit(1);
    }

    zBuffer = (float*)malloc(sizeof(float) * WINDOW_W * WINDOW_H);
    if (!zBuffer) {
        fprintf(stderr, "Error allocating memory for z buffer.\n");
        exit(1);
    }

    // Light
    light.direction = vec3normalize((Vec3f){0, 0, 1});

    // Create a SDL texture
    // SDL_PIXELFORMAT_ARGB8888 is a 32-bit pixel format, with alpha, red, green, and blue channels
    // check https://wiki.libsdl.org/SDL_CreateTexture for more information
    // SDL_TEXTUREACCESS_STREAMING means that the texture will be updated frequently
    colorBufferTexture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            WINDOW_W,
            WINDOW_H
    );

    // Get the hardcoded values for the texture
    // brickTexture = (uint32_t* )REDBRICK_TEXTURE;
    // 0xEEAE00

    // loadObjFile("assets/cube.obj");
    // loadCubeMeshData();
    if (loadObjFile("assets/new assets/cube.obj", &mesh) == -1) {
        printf("Error loading the mesh file\n");
        exit(1);
    }


    if (loadTextureFile("assets/new assets/cube.png", &meshTextureWidth, &meshTextureHeight) == -1) {
        printf("Error loading the png file\n");
        exit(1);
    }

    // Initialize the perspective projection matrix
    float fov = M_PI/3.0;// 60.0 deg in rads
    float aspect = (float)WINDOW_H/(float)WINDOW_W;
    float znear = 0.1;
    float zfar = 500.0;
    createPerspectiveMatrix(&perspectiveMatrix, fov, aspect, znear, zfar);

    // Initialize the frustum
    initializeFrustumPlanes(fov, znear, zfar);

    // Drawing context for the clipping tool (used for debugging)
    dc.boundX = WINDOW_W;
    dc.boundY = WINDOW_H;
    dc.buffer = colorBuffer;
    dc.bufferWidth = WINDOW_W;
    setDrawingContext_clipping(&dc);
}

void processInput(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
    
        switch(event.type) {
            case SDL_QUIT: {
                isRunning = false;
                break;
            }
            case SDL_KEYDOWN: {
                SDL_KeyCode key = event.key.keysym.sym;
                if (key == SDLK_ESCAPE) {
                    isRunning = false;
                }

                if (isRenderOption(key)) {
                    renderMode = (RenderMode)key;
                }

                if (key == BACK_FACE_CULLING || key == BACK_FACE_CULLING_DISABLED) {
                    hasBackFaceCulling = (key == BACK_FACE_CULLING) ? true : false;
                }

                if (isMovementDirection(key)) {
                    moveCamera((MovementDirection)key);
                }
            }
            default: {
                break;
            }
        }

    }
}

void renderLine(Vec2f v1, Vec2f v2, uint32_t color) {
    drawLine(v1, v2, color, colorBuffer, WINDOW_W, WINDOW_W, WINDOW_H);
}

void render(void) {
    // for (int i = numTrianglesToRender - 1; i >= 0; i--) {
    for (int i = 0; i < numTrianglesToRender; i++) {
        Triangle triangle = trianglesToRender[i];
        Vec2f tPoints[] = {
            (Vec2f){triangle.points[0].x, triangle.points[0].y},
            (Vec2f){triangle.points[1].x, triangle.points[1].y},
            (Vec2f){triangle.points[2].x, triangle.points[2].y}
        };
        if (renderMode == FILL || renderMode == FILLED_OUTLINE) {
            drawTriangleFill(triangle.points, triangle.color, colorBuffer, WINDOW_W, WINDOW_W, WINDOW_H);
        }
        
        if (renderMode == TEXTURE || renderMode == TEXTURE_OUTLINE) {
            drawTriangleBar(triangle.points, triangle.uvTexture, mesh_texture, meshTextureHeight, meshTextureWidth, colorBuffer, WINDOW_W, WINDOW_W, WINDOW_H);
        }

        if (renderMode == OUTLINE || renderMode == FILLED_OUTLINE || renderMode == TEXTURE_OUTLINE || renderMode == POINTS_OUTLINE) {
            drawTriangle(tPoints, OUTLINE_COLOR, colorBuffer, WINDOW_W, WINDOW_W, WINDOW_H);
        }

        if (renderMode == POINTS || renderMode == POINTS_OUTLINE) {
            rect(triangle.points[0].x, triangle.points[0].y, 3, 3, GREEN);
            rect(triangle.points[1].x, triangle.points[1].y, 3, 3, GREEN);
            rect(triangle.points[2].x, triangle.points[2].y, 3, 3, GREEN);
        }
    }

    // Update the texture with new color buffer
    renderColorBuffer(); // Render the color buffer to the screen
    // 0X00000000
    clearColorBuffer(BACKGROUND_COLOR);
    clearZBuffer();
    SDL_RenderPresent(renderer);
}
void freeResources() {
    textureFree();
    free(colorBuffer);
    free(zBuffer);
    matrixFree(perspectiveMatrix);
    // Dynamic array free 
    array_free(mesh.faces);
}

void moveCamera(MovementDirection key) {
    switch(key) {
        case UP: {
            camera.position.y += 5 * dt;
            break;
        }
        case DOWN: {
            camera.position.y -= 5 * dt;
            break;
        }
        case LEFT: {
            camera.yaw += 2.0 * dt;
            break;
        }
        case RIGHT: {
            camera.yaw -= 2.0 * dt;
            break;
        }
        case FORWARD: {
            camera.forwardVelocity = vec3mult(camera.direction, 5 * dt);
            camera.position = vec3add(camera.position, camera.forwardVelocity);
            break;
        }
        case BACKWARD: {
            camera.forwardVelocity = vec3mult(camera.direction, 5 * dt);
            camera.position = vec3sub(camera.position, camera.forwardVelocity);
            break;
        }
    }
}
