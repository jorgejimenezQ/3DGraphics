#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "utils.h"

int loadObjFile(char* filepath, Mesh* mesh) {
      // Load the model
    /**
     * aiImportFile() is the function that loads the model from the file.
     * The first argument is the path to the file.
     * The second argument is a bitwise OR of the flags that specify how the model should be processed:
     * - aiProcess_Triangulate: Triangulates the model, converting all polygons to triangles.
     * - aiProcess_FlipUVs: Flips the texture coordinates on the y-axis.
     * - aiProcess_CalcTangentSpace: Calculates the tangent and bitangent vectors for the model.
     * - aiProcess_FlipWindingOrder: Flips the winding order of the faces.
     * - aiProcess_GenNormals: Generates normals for the model if they are not present.
     */
    const struct aiScene* scene = aiImportFile(filepath, 
        aiProcess_FlipUVs 
        // | aiProcess_FlipWindingOrder
    );

    if (!scene) {
        printf("ERROR::ASSIMP:: %s\n", aiGetErrorString());
        return -1;
    }

    // Output some basic information
    printf("Model loaded: %s\n", filepath);
    printf("Number of meshes: %u\n", scene->mNumMeshes);
    printf("Number of materials: %u\n", scene->mNumMaterials);

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        printf("Mesh %u: %u vertices, %u faces\n", i, scene->mMeshes[i]->mNumVertices, scene->mMeshes[i]->mNumFaces);
        // Get the faces into the mesh
        for (unsigned int j = 0; j < scene->mMeshes[i]->mNumFaces; j++) {
            const struct aiMesh* currentMesh = scene->mMeshes[i];
            const struct aiFace* face = &currentMesh->mFaces[j];
            const struct aiVector3D* vertices = currentMesh->mVertices;
            const struct aiVector3D* normals = currentMesh->mNormals;
            const struct aiVector3D* textures = currentMesh->mTextureCoords[0];

            Vec3f fPoints[3] = {
                { .x = vertices[face->mIndices[0]].x, .y = vertices[face->mIndices[0]].y, .z = vertices[face->mIndices[0]].z },
                { .x = vertices[face->mIndices[1]].x, .y = vertices[face->mIndices[1]].y, .z = vertices[face->mIndices[1]].z },
                { .x = vertices[face->mIndices[2]].x, .y = vertices[face->mIndices[2]].y, .z = vertices[face->mIndices[2]].z }
            };

            

            Face f = {
                .points = { fPoints[0], fPoints[1], fPoints[2] },
            };

            if (currentMesh->mNumUVComponents[0] > 0) {
                Vec2f uvTexture[3] = {
                    { .x = textures[face->mIndices[0]].x, .y = textures[face->mIndices[0]].y },
                    { .x = textures[face->mIndices[1]].x, .y = textures[face->mIndices[1]].y },
                    { .x = textures[face->mIndices[2]].x, .y = textures[face->mIndices[2]].y }
                };

                f.uvTexture[0] = uvTexture[0];
                f.uvTexture[1] = uvTexture[1];
                f.uvTexture[2] = uvTexture[2];
            }

            if (currentMesh->mNormals != NULL) {
                Vec3f fNormals[3] = {
                    { .x = normals[face->mIndices[0]].x, .y = normals[face->mIndices[0]].y, .z = normals[face->mIndices[0]].z },
                    { .x = normals[face->mIndices[1]].x, .y = normals[face->mIndices[1]].y, .z = normals[face->mIndices[1]].z },
                    { .x = normals[face->mIndices[2]].x, .y = normals[face->mIndices[2]].y, .z = normals[face->mIndices[2]].z }
                };

                f.normals[0] = fNormals[0];
                f.normals[1] = fNormals[1];
                f.normals[2] = fNormals[2];
            }

            array_push(mesh->faces, f);
            mesh->numVertices += 3;;
            mesh->numFaces++;
        }
    }

    // matrixCreate(4, mesh->numVertices, &mesh->vertices);
    // for (int f = 0; f < mesh->numFaces; f++) {
    //     Vec3f v1 = mesh->faces[f].points[0];
    //     Vec3f v2 = mesh->faces[f].points[1];
    //     Vec3f v3 = mesh->faces[f].points[2];

    //     matrixLoadVec4f(mesh->vertices, f, createVec4f(v1.x, v1.y, v1.z, 1.0f));
    //     matrixLoadVec4f(mesh->vertices, f+1, createVec4f(v2.x, v2.y, v2.z, 1.0f));
    //     matrixLoadVec4f(mesh->vertices, f+2, createVec4f(v3.x, v3.y, v3.z, 1.0f));
    // }


    printf("Number of materials: %u\n", scene->mNumMaterials);
    for (unsigned k = 0; k < scene->mNumMaterials; k++) {
        const struct aiMaterial* material = scene->mMaterials[k];
        // Check for diffuse texture
        if (aiGetMaterialTextureCount(material, aiTextureType_DIFFUSE) > 0) {
            printf("  Has diffuse texture.\n");

            struct aiString path;
            if (AI_SUCCESS == aiGetMaterialTexture(material, aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL, NULL)) {
                printf("  Diffuse texture path: %s\n", path.data);
            }
        } else {
            printf("  No diffuse texture.\n");
        }

        // Get and print diffuse color
        struct aiColor4D color;
        if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color)) {
            printf("  Diffuse color: (%f, %f, %f, %f)\n", color.r, color.g, color.b, color.a);
        }
    }

    aiReleaseImport(scene); // Don't forget to release the scene!
    return 1;
}

// typedef struct {
//     Vec3f points[3];
//     Vec3f normals[3];
//     Vec2f uvTexture[3];
// } Face;
