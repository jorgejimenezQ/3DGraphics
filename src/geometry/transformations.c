#include "transformations.h"

// | sx  0   0   0 |
// | 0   sy  0   0 |
// | 0   0   sz  0 |
// | 0   0   0   1 |
void  createScaleMatrix(Matrix *m, float sx, float sy, float sz) {
    matrixIdentity(4, 4, m);
    m->data[0]  = sx;
    m->data[5]  = sy;
    m->data[10] = sz;

    return;
}

// | 0   0   0   tx |
// | 0   0   0   ty |
// | 0   0   0   tz |
// | 0   0   0   1  |
void createTranslationMatrix(Matrix *m, float tx, float ty, float tz) { 
    matrixIdentity(4, 4, m);
    m->data[3]  = tx;
    m->data[7]  = ty;
    m->data[11] = tz;
    return ;
}

/**
 * Rotation matrix about the x, y, or z axis.
 * 
 * Rotation matrix about the x axis:
 | 1   0   0   0 | \n
 | 0   c  -s   0 | \n
 | 0   s   c   0 | \n
 | 0   0   0   1 | \n
 
 Rotation matrix about the y axis:
 |  c  0   s   0 | \n
 |  0  1   0   0 | \n
 | -s  0   c   0 | \n
 |  0  0   0   1 | \n
  
 Rotation matrix about the z axis:
 |  c  -s  0   0 | \n
 |  s   c  0   0 | \n
 |  0   0  1   0 | \n
 |  0   0  0   1 | \n
 * 
 * @param m     Pointer to a matrix struct
 */
void  createRotationMatrix(Matrix *m, float angle, char axis) {
    matrixIdentity(4, 4, m);
    float c = cos(angle);
    float s = sin(angle);

    switch(axis) {
        case 'x':
            m->data[5] = c;
            m->data[6] = -s;
            m->data[9] = s;
            m->data[10] = c;
            break;
        case 'y':
            m->data[0] = c;
            m->data[2] = s;
            m->data[8] = -s;
            m->data[10] = c;
            break;
        case 'z':
            m->data[0] = c;
            m->data[1] = -s;
            m->data[4] = s;
            m->data[5] = c;
            break;
        default:
            break;
    }

    return;
}

// | f/a  0          0          0        |
// | 0    f          0          0        |
// | 0    0    (f)/(f - n)  -(f)/(f-n)*n |
// | 0    0          1          0        |
void createPerspectiveMatrix(Matrix *m, float fov, float aspect, float near, float far) {
    matrixIdentity(4, 4, m);
    float f = 1.0f / tan(fov / 2.0f);
    m->data[0] = aspect * f;
    m->data[5] = f;
    m->data[10] = far / (far - near);
    m->data[11] = -far * near / (far - near);
    m->data[14] = 1.0f;
    return;
}

// Divide the projection matrix by the w component of the vector
// This function imposes the following constraints:
// 1. The projection matrix must be a 4x4 matrix
// 2. The view matrix must be a 4x1 matrix
// 3. The projection matrix must be multiplied by the view matrix
void projectionDivide(Matrix *out, Matrix *projection, Matrix *view) {
    // out =
    // | x |
    // | y |
    // | z |
    // | w |

    matrixMult(*projection, *view, out);
    if (out->data == NULL) {
        return;
    }

    if (out->data[3] == 0) {
        return;
    }

    out->data[0] /= out->data[3];
    out->data[1] /= out->data[3];
    out->data[2] /= out->data[3];
}

// | x.x  x.y  x.z  -x.x*eye.x - x.y*eye.y - x.z*eye.z |
// | y.x  y.y  y.z  -y.x*eye.x - y.y*eye.y - y.z*eye.z |
// | z.x  z.y  z.z  -z.x*eye.x - z.y*eye.y - z.z*eye.z |
// | 0    0    0    1                                  |
void createLookAt(Vec3f eye, Vec3f target, Vec3f up, Matrix *m) {
    Vec3f z = vec3normalize(vec3sub(target, eye));
    Vec3f x = vec3normalize(vec3cross(up, z));
    Vec3f y = vec3cross(z, x);

    m->data[0]  = x.x,   m->data[1]  = x.y,    m->data[2]   = x.z,   m->data[3]   = -vec3dot(x, eye);
    m->data[4]  = y.x,   m->data[5]  = y.y,    m->data[6]   = y.z,   m->data[7]   = -vec3dot(y, eye);
    m->data[8]  = z.x,   m->data[9]  = z.y,    m->data[10]  = z.z,   m->data[11]  = -vec3dot(z, eye);
    m->data[12] = 0,     m->data[13] = 0,      m->data[14]  = 0,      m->data[15] = 1;
}



void transformFace(Face face, Matrix *worldMatrix, Matrix *viewMatrix, Matrix *out) {
    Matrix currentVertex;
    matrixCreate(4, 1, &currentVertex);
    Matrix transformedVertex;
    matrixCreate(4, 1, &transformedVertex);
    for (int i = 0; i < 3; i++) {
        /******************************************************/
        // TRANSFORMATIONS
        currentVertex.data[0] = face.points[i].x;
        currentVertex.data[1] = face.points[i].y;
        currentVertex.data[2] = face.points[i].z;
        currentVertex.data[3] = 1;

        // Rotate Matrix
        matrixMult(*worldMatrix, currentVertex, &transformedVertex);
        // Apply the view matrix
        matrixMult(*viewMatrix, transformedVertex, &transformedVertex);


        // Save the transformed vertex in the array of transformed vertices
        out[i].data = NULL;
        matrixCopy(transformedVertex, &out[i]);
    }
    matrixFree(currentVertex);
    matrixFree(transformedVertex); 
}

bool isBackface(Matrix *face, Vec3f *normal) {
    Vec3f vec3Vertices[3];
    for (int k = 0; k < 3; k++) {
        vec3Vertices[k].x = face[k].data[0];
        vec3Vertices[k].y = face[k].data[1];
        vec3Vertices[k].z = face[k].data[2];
    }

    Vec3f v1 = vec3sub(vec3Vertices[1], vec3Vertices[0]);
    Vec3f v2 = vec3sub(vec3Vertices[2], vec3Vertices[0]);
    vec3normalizeRef(&v1);
    vec3normalizeRef(&v2);

    Vec3f faceNormal = vec3cross(v1, v2);
    vec3normalizeRef(&faceNormal);
    *normal = faceNormal;
    // Calculate the vector from the camera position to the first vertex of the triangle
    Vec3f origin = {0, 0, 0};
    Vec3f camRay = vec3sub(origin, vec3Vertices[0]);
    float dotProd = vec3dot(faceNormal, camRay);

    return (dotProd <= 0);
}

void projectTriangle(Matrix *transformedVertices, Matrix *perspectiveMatrix, int WINDOW_W, int WINDOW_H, Triangle *out) {
    Matrix projectedVertex;
    matrixCreate(4, 1, &projectedVertex);
    for (int j = 0; j < 3; j++) {
        projectionDivide(&projectedVertex, perspectiveMatrix, &transformedVertices[j]);

        // scale
        projectedVertex.data[0] *= (WINDOW_W/2);
        projectedVertex.data[1] *= (WINDOW_H/2);

        // Invert the y values to account for flipped screen y coordinate
        projectedVertex.data[1] *= -1;
        // translate to the middle of the screen
        projectedVertex.data[0] += (WINDOW_W/2);
        projectedVertex.data[1] += (WINDOW_H/2);

        // Save the projected triangle
        out->points[j].x = projectedVertex.data[0];
        out->points[j].y = projectedVertex.data[1];
        out->points[j].z = projectedVertex.data[2];
        out->points[j].w = projectedVertex.data[3];

        // Get the average of all the z's 
        float zAvg = (transformedVertices[0].data[2] + transformedVertices[1].data[2] + transformedVertices[2].data[2]) / 3.0;
        out->avgDepth =  zAvg;
    }
    matrixFree(projectedVertex);
}