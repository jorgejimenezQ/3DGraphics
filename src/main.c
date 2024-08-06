#include "array.h"
#include "./display/display.h"
#include "errors/errors.h"
#include "./draw/draw.h"
#include "./geometry/geometry.h"
#include "./geometry/mesh.h"
#include "./utils/utils.h"

/************************************************************************ */
// Array of triangles to render on the screen
/************************************************************************ */
Triangle* trianglesToRender = NULL;

int actualFPS = 0;
Uint32 frameStart;
int frameTime;
// Color buffer
float fovFactor = 800;
bool isRunning = false;
int prevFrameTime;
Vec3f camPosition = { 0, 0, 0};
// array of three random colors
uint32_t BACKGROUND_COLOR = 0XFF555555;
uint32_t FOREGROUND_COLOR = 0XFF8888AA;
uint32_t OUTLINE_COLOR = 0XFF00d000;
uint32_t RED = 0XFFFF0000;
// Render mode
char renderMode = '4';
bool hasBackFaceCulling = true;
char transformations = '.';

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

    // Create a SDL texture
    // SDL_PIXELFORMAT_ARGB8888 is a 32-bit pixel format, with alpha, red, green, and blue channels
    // check https://wiki.libsdl.org/SDL_CreateTexture for more information
    // SDL_TEXTUREACCESS_STREAMING means that the texture will be updated frequently
    colorBufferTexture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            WINDOW_W,
            WINDOW_H
    );

    // loadObjFile("assets/cube.obj");
    loadCubeMeshData();
}

void processInput(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    
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

            // Render type
            if (key == '0' || 
                key == '1' || 
                key == '2' || 
                key == '3' || 
                key == '4') {
                    renderMode = key;
            } 

            // Back Face Culling
            if (key == 'c' || key == 'd') hasBackFaceCulling = key == 'c';

            // Transformations
            if (key == 'j' || 
                key == 'k' || 
                key == 'h' || 
                key == 'l' || 
                key == 'z' ||
                key == 'Z' ||
                key == 'r' ||
                key == '.') { 

                    transformations = key;
            } 
        }
        default: {
            break;
        }
    }
}

/*****************************************************/
// Takes a 3D vector and returns a projected 2d point
Vec2f project(Vec3f point ) {
    Vec2f projectedPoint = {
        .x = (fovFactor * point.x) / point.z,
        .y = (fovFactor * point.y) / point.z
    };
    
    return projectedPoint;
}

void processTransformation() {
    if (transformations == 'j') mesh.rotation.x -= 0.05;
    if (transformations == 'k') mesh.rotation.x += 0.05;
    if (transformations == 'h') mesh.rotation.y -= 0.05;
    if (transformations == 'l') mesh.rotation.y += 0.05;
    if (transformations == 'z') mesh.rotation.z += 0.05;
    if (transformations == 'Z') mesh.rotation.z -= 0.05;
    
    if (transformations == 'r') {
        mesh.rotation.x = 0;
        mesh.rotation.y = 0;
        mesh.rotation.z = 0;
    }

    if (transformations == '.') {
        mesh.rotation.x += 0;
        mesh.rotation.y += 0;
    }

    // Keep the rotation within 360 degrees
    if (mesh.rotation.x > 360 || mesh.rotation.x < -360) mesh.rotation.x = 0;
    if (mesh.rotation.y > 360 || mesh.rotation.y < -360) mesh.rotation.y = 0;
    if (mesh.rotation.z > 360 || mesh.rotation.z < -360) mesh.rotation.z = 0;
}

void update(void) {
    trianglesToRender = NULL;

    processTransformation();

    int numFaces = array_length(mesh.faces);
    bool isBackFace = false;

    // mesh.rotation.x += 0.01;
    // mesh.rotation.y += 0.01;
    // mesh.rotation.z += 0.01;

    // Get all the faces
    for (int i = 0; i < numFaces; i++) {
        float zAvg;
        Face currentFace = mesh.faces[i];
        Vec3f faceVertices[3] = {
            mesh.vertices[currentFace.a - 1], 
            mesh.vertices[currentFace.b - 1], 
            mesh.vertices[currentFace.c - 1] 
        };

        Triangle projectedTriangle;
        Vec3f transformedVertices[3];

        for (int j = 0; j < 3; j++) {
            /******************************************************/
            // TRANSFORMATIONS
            Vec3f vertex = faceVertices[j];

            Vec3f transformedVertex = rotateX(vertex, mesh.rotation.x);
            transformedVertex = rotateY(transformedVertex, mesh.rotation.y);
            transformedVertex = rotateZ(transformedVertex, mesh.rotation.z);
            // Translate  the vertex away fro the camera
            transformedVertex.z += 5;

            // Save the transformed vertex in the array of transformed vertices
            transformedVertices[j] = transformedVertex;
        
            /******************************************************/
            // BACK FACE CULLING
            if (hasBackFaceCulling && j == 2) {
                Vec3f v1 = vec3sub(transformedVertices[1], transformedVertices[0]);
                Vec3f v2 = vec3sub(transformedVertices[2], transformedVertices[0]);
                Vec3f faceNormal = vec3cross(v1, v2);
                vec3normalizeRef(&faceNormal);

                Vec3f camRay = vec3sub(camPosition, transformedVertices[0]);
                if (vec3dot(faceNormal, camRay) < 0) {
                    isBackFace = true;
                    break;
                }
            }

            // Get the average of all the z's 
            zAvg = (transformedVertices[0].z + transformedVertices[1].z + transformedVertices[2].z) / 3.0;

            /******************************************************/
            // PROJECTION
            // Project the current vertex
            Vec2f projectedVertex = project(transformedVertices[j]);

            // Scale and translate to the middle of the screen
            projectedVertex.x += (WINDOW_W/2);
            projectedVertex.y += (WINDOW_H/2);

            // Save the projected triangle
            projectedTriangle.points[j] = projectedVertex;
        } // end of for loop - vertices

        if (isBackFace) {
            isBackFace = false;
            continue;
        }

        projectedTriangle.color = currentFace.color;
        projectedTriangle.avgDepth = zAvg;

        // our array of triangles
        // trianglesToRender[i] = projectedTriangle;
        array_push(trianglesToRender, projectedTriangle);
    } // end of for loop - faces

    // Sort the triangles by their average depth
    sort(trianglesToRender);
}

void renderLine(Vec2f v1, Vec2f v2, uint32_t color) {
    drawLine(v1, v2, color, colorBuffer, WINDOW_W, WINDOW_W, WINDOW_H);
}

void render(void) {
    int numTriangles = array_length(trianglesToRender);
    for (int i = 0; i < numTriangles; i++) {
        Triangle triangle = trianglesToRender[i];

        if (renderMode == '1') {
            drawTriangle(triangle.points, OUTLINE_COLOR, colorBuffer, WINDOW_W, WINDOW_W, WINDOW_H);
            rect(triangle.points[0].x, triangle.points[0].y, 3, 3, RED);
            rect(triangle.points[1].x, triangle.points[1].y, 3, 3, RED);
            rect(triangle.points[2].x, triangle.points[2].y, 3, 3, RED);
        } else if (renderMode == '2') {
            drawTriangle(triangle.points, OUTLINE_COLOR, colorBuffer, WINDOW_W, WINDOW_W, WINDOW_H);
        } else if (renderMode == '3') {
            drawTriangleFill(triangle.points, triangle.color, colorBuffer, WINDOW_W, WINDOW_W, WINDOW_H);
        } else if (renderMode == '4') {
            drawTriangleFill(triangle.points, triangle.color, colorBuffer, WINDOW_W, WINDOW_W, WINDOW_H);
            drawTriangle(triangle.points, OUTLINE_COLOR, colorBuffer, WINDOW_W, WINDOW_W, WINDOW_H);
        } 
    }

    array_free(trianglesToRender);
    // Update the texture with new color buffer
    renderColorBuffer(); // Render the color buffer to the screen

    // 0X00000000
    clearColorBuffer(BACKGROUND_COLOR);

    SDL_RenderPresent(renderer);
}
void freeResources() {
    free(colorBuffer);
    // Dynamic array free 
    array_free(mesh.faces);
    array_free(mesh.vertices);
}

int main(void) {
    isRunning = initWindow(500, 600);

    setup();

    Uint32 startTime = SDL_GetTicks();
    int frameCount = 0;

    while(isRunning) {
        frameStart = SDL_GetTicks();

        processInput();
        update();
        render();

        frameCount++;
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - startTime >= 1000) {
            actualFPS = frameCount;
            frameCount = 0;
            startTime = currentTime;
            // Clear the terminal screen before printing the actual FPS
            system("clear");
            printf("Actual FPS: %d\n", actualFPS);
        }

        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_TARGET_TIME) {
            SDL_Delay(FRAME_TARGET_TIME - frameTime);
        }
    }

    freeResources();
    destroyWindow();

    return 0;
}