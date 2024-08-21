#ifndef MATRIX_H
#define MATRIX_H

// Include any necessary headers
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

// Define your class or struct
typedef struct {
    int rows;
    int cols;
    float *data;
} Matrix;

// Declare your class or struct
Matrix matrixCreate(int rows, int cols);
// Put data in the matrix
Matrix matrixCreateWithData(int rows, int cols, float *data);

// Declare any function prototypes
Matrix matrixAdd(Matrix m1, Matrix m2);
Matrix matrixSub(Matrix m1, Matrix m2);
Matrix matrixMult(Matrix m1, Matrix m2);
Matrix matrixScalarMult(Matrix m, float k);
Matrix matrixTranspose(Matrix m);
Matrix matrixInverse(Matrix m);
// void matrixRow(Matrix *m, int row, float *data);
Matrix matrixIdentity(int rows, int cols);
Matrix matrixCopy(Matrix m);
void matrixPrint(Matrix m);
void matrixFree(Matrix m);
void matrixExample(void);

#endif // MATRIX_H