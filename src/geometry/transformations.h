#ifndef FILENAME_H
#define FILENAME_H

// Include any necessary headers
#include "matrix.h"

// Declare any function prototypes
void  createScaleMatrix(Matrix *m, float sx, float sy, float sz);
void  createTranslationMatrix(Matrix *m, float tx, float ty, float tz);
void  createRotationMatrix(Matrix *m, float angle, char axis);




#endif // FILENAME_H