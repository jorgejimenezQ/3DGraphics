#ifndef FILENAME_H
#define FILENAME_H

// Include any necessary headers
#include "matrix.h"
#include <math.h>

// Declare any function prototypes
void  createScaleMatrix(Matrix *m, float sx, float sy, float sz);
void  createTranslationMatrix(Matrix *m, float tx, float ty, float tz);
void  createRotationMatrix(Matrix *m, float angle, char axis);
// void createShearMatrix(Matrix *m, float shx, float shy, float shz);
void createPerspectiveMatrix(Matrix *m, float fov, float aspect, float near, float far);
void projectionDivide(Matrix *m, Matrix *projection, Matrix *view);
#endif // FILENAME_H