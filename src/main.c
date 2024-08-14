#include "array.h"
#include "./display/display.h"
#include "errors/errors.h"
#include "./draw/draw.h"
#include "./geometry/geometry.h"
#include "./geometry/mesh.h"
#include "./utils/utils.h"
#include "./geometry/matrix.h"
#include "./geometry/transformations.h"

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
Matrix perspectiveMatrix;

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

    // loadObjFile("assets/f22.obj");
    loadCubeMeshData();


    // Initialize the perspective projection matrix
    float fov = M_PI/3.0;// 60.0 deg in rads
    float aspect = (float)WINDOW_H/(float)WINDOW_W;
    float znear = 0.1;
    float zfar = 100.0;
    createPerspectiveMatrix(&perspectiveMatrix, fov, aspect, znear, zfar);
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

    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;

    mesh.translation.z = 10;

    // mesh.scale.x += 0.002;
    // mesh.scale.y += 0.002;
    // mesh.scale.z += 0.002;

    // Scale matrix
    Matrix scaleMatrix;
    Matrix translationMatrix;
    Matrix xRotationMatrix;
    Matrix yRotationMatrix;
    Matrix zRotationMatrix;
    createScaleMatrix(&scaleMatrix, mesh.scale.x, mesh.scale.y, mesh.scale.z);
    createTranslationMatrix(&translationMatrix, mesh.translation.x, mesh.translation.y, mesh.translation.z);
    createRotationMatrix(&xRotationMatrix, mesh.rotation.x, 'x');
    createRotationMatrix(&yRotationMatrix, mesh.rotation.y, 'y');
    createRotationMatrix(&zRotationMatrix, mesh.rotation.z, 'z');
    
    Matrix worldMatrix = matrixIdentity(4, 4);
    worldMatrix = matrixMult(xRotationMatrix, worldMatrix);
    worldMatrix = matrixMult(yRotationMatrix, worldMatrix);
    worldMatrix = matrixMult(zRotationMatrix, worldMatrix);
    worldMatrix = matrixMult(scaleMatrix, worldMatrix);
    worldMatrix = matrixMult(translationMatrix, worldMatrix);

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
        // Vec3f transformedVertices[3];
        Matrix transformedVertices[3];
        // The current vertex as a column vertex
        Matrix currentVertex = matrixCreate(4, 1);
        for (int j = 0; j < 3; j++) {
            Matrix transformedVertex;

            /******************************************************/
            // TRANSFORMATIONS
            currentVertex.data[0] = faceVertices[j].x;
            currentVertex.data[1] = faceVertices[j].y;
            currentVertex.data[2] = faceVertices[j].z;
            currentVertex.data[3] = 1;

            // Rotate Matrix
            transformedVertex = matrixMult(worldMatrix, currentVertex);

            // Save the transformed vertex in the array of transformed vertices
            transformedVertices[j] = matrixCopy(transformedVertex);
            matrixFree(transformedVertex);
           
            /******************************************************/
            // BACK FACE CULLING
            // Perform back face culling if enabled and it's the third vertex of the triangle
            if (hasBackFaceCulling && j == 2) {
                // Calculate the face normal
                Vec3f vec3Vertices[3];
                for (int k = 0; k < 3; k++) {
                    vec3Vertices[k].x = transformedVertices[k].data[0];
                    vec3Vertices[k].y = transformedVertices[k].data[1];
                    vec3Vertices[k].z = transformedVertices[k].data[2];
                }

                Vec3f v1 = vec3sub(vec3Vertices[1], vec3Vertices[0]);
                Vec3f v2 = vec3sub(vec3Vertices[2], vec3Vertices[0]);
                Vec3f faceNormal = vec3cross(v1, v2);
                vec3normalizeRef(&faceNormal);

                // Calculate the vector from the camera position to the first vertex of the triangle
                Vec3f camRay = vec3sub(camPosition, vec3Vertices[0]);

                // Check if the face is facing away from the camera
                if (vec3dot(faceNormal, camRay) < 0) {
                    isBackFace = true;
                    break;
                }
            }

            /******************************************************/
            // PROJECTION
            // Project the current vertex
            Matrix projectedVertex;
            projectionDivide(&projectedVertex, &perspectiveMatrix, &transformedVertices[j]);

            // scale
            projectedVertex.data[0] *= (WINDOW_W/2);
            projectedVertex.data[1] *= (WINDOW_H/2);
            // translate to the middle of the screen
            projectedVertex.data[0] += (WINDOW_W/2);
            projectedVertex.data[1] += (WINDOW_H/2);


            // Save the projected triangle
            projectedTriangle.points[j].x = projectedVertex.data[0];
            projectedTriangle.points[j].y = projectedVertex.data[1];
        } // end of for loop - vertices

        if (isBackFace) {
            isBackFace = false;
            continue;
        }

        projectedTriangle.color = currentFace.color != 0 ? currentFace.color : FOREGROUND_COLOR;

        
        // Get the average of all the z's 
        zAvg = (transformedVertices[0].data[2] + transformedVertices[1].data[2] + transformedVertices[2].data[2]) / 3.0;
        projectedTriangle.avgDepth = zAvg;

        // our array of triangles
        // trianglesToRender[i] = projectedTriangle;
        array_push(trianglesToRender, projectedTriangle);
        for (int k = 0;k < 3; k++) {
            matrixFree(transformedVertices[k]);
        }
        matrixFree(currentVertex);

    } // end of for loop - faces

    // Sort the triangles by their average depth
    sort(trianglesToRender);

    // Free the matrices
    matrixFree(scaleMatrix);
    matrixFree(translationMatrix);
    matrixFree(xRotationMatrix);
    matrixFree(yRotationMatrix);
    matrixFree(zRotationMatrix);
}

void renderLine(Vec2f v1, Vec2f v2, uint32_t color) {
    drawLine(v1, v2, color, colorBuffer, WINDOW_W, WINDOW_W, WINDOW_H);
}

void render(void) {
    int numTriangles = array_length(trianglesToRender);
    for (int i = numTriangles - 1; i >= 0; i--) {
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
    matrixFree(perspectiveMatrix);
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