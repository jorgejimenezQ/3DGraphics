#include "transformations.h"

// | sx  0   0   0 |
// | 0   sy  0   0 |
// | 0   0   sz  0 |
// | 0   0   0   1 |
void  createScaleMatrix(Matrix *m, float sx, float sy, float sz) {
    *m = matrixIdentity(4, 4);
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
    *m = matrixIdentity(4, 4);
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
    *m = matrixIdentity(4, 4);
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
    *m = matrixIdentity(4, 4);
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
void projectionDivide(Matrix *m, Matrix *projection, Matrix *view) {
    // m =
    // | x |
    // | y |
    // | z |
    // | w |

    *m = matrixMult(*projection, *view);
    if (m->data == NULL) {
        return;
    }

    if (m->data[3] == 0) {
        return;
    }

    m->data[0] /= m->data[3];
    m->data[1] /= m->data[3];
    m->data[2] /= m->data[3];
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