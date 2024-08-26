#ifndef MATRIX_H
#define MATRIX_H

// Include any necessary headers
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include "./geometry.h"

// Define your class or struct
typedef struct {
    int rows;
    int cols;
    float *data;
} Matrix;

// Declare your class or struct
void matrixCreate(int rows, int cols, Matrix *m);
// Put data in the matrix
void matrixCreateWithData(int rows, int cols, float *data, Matrix *m);
// Declare any function prototypes
void matrixAdd(Matrix m1, Matrix m2, Matrix *out);
void matrixSub(Matrix m1, Matrix m2, Matrix *out);
void matrixMult(Matrix m1, Matrix m2, Matrix *out);
void matrixScalarMult(Matrix m, float k, Matrix *out);
void matrixTranspose(Matrix m, Matrix *out);
void matrixInverse(Matrix m, Matrix *out);
void matrixIdentity(int rows, int cols, Matrix *out);
void matrixCopy(Matrix m, Matrix *out);
void matrixLoadColumn(Matrix m, int col, float *data);
void matrixLoadVec3f(Matrix m, int col, Vec3f data);
void matrixLoadVec4f(Matrix m, int col, Vec4f data);
Vec3f matrixGetVec3f(Matrix m, int col);
Vec4f matrixGetVec4f(Matrix m, int col);

// void matrixRow(Matrix *m, int row, float *data);
void matrixPrint(Matrix m);
void matrixFree(Matrix m);
// void matrixExample(void);

void matrixCreateFromV4(Vec4f v, Matrix *m);
#endif // MATRIX_H