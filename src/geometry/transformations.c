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