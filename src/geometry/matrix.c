#include "matrix.h"

void matrixCreate(int rows, int cols, Matrix *m) {
    m;
    m->rows = rows;
    m->cols = cols;
    m->data = NULL;
    if (rows*cols == 0) return;

    m->data = (float *)malloc(rows * cols * sizeof(float));
}

void matrixCreateWithData(int rows, int cols, float *data, Matrix *m) {
    // Check the number of arguments
    if (data == NULL) {
        printf("Data cannot be NULL\n");
        m = NULL;
    }

    matrixCreate(rows, cols, m);
    for (int i = 0; i < rows * cols; i++) {
        m->data[i] = data[i];
    }
}

void matrixCreateFromV4(Vec4f v, Matrix *m) {
    matrixCreate(4, 1, m);
    m->data[0] = v.x;
    m->data[1] = v.y;
    m->data[2] = v.z;
    m->data[3] = v.w;
}

void matrixAdd(Matrix m1, Matrix m2, Matrix *out) {
    if (m1.rows != m2.rows || m1.cols != m2.cols) {
        printf("Matrix dimensions do not match\n");
    }

    Matrix temp;
    matrixCreate(m1.rows, m1.cols, &temp);
    for (int i = 0; i < m1.rows * m1.cols; i++) {
        temp.data[i] = m1.data[i] + m2.data[i];
    }

    matrixCopy(temp, out);
    matrixFree(temp);
}

void matrixSub(Matrix m1, Matrix m2, Matrix *out) {
    if (m1.rows != m2.rows || m1.cols != m2.cols) {
        printf("Matrix dimensions do not match\n");
        // exit(1);
        *out = (Matrix){0, 0, NULL};
    }

    Matrix temp;
    matrixCreate(m1.rows, m1.cols, &temp);
    for (int i = 0; i < m1.rows * m1.cols; i++) {
        temp.data[i] = m1.data[i] - m2.data[i];
    }
    
    matrixCopy(temp, out);
    matrixFree(temp);
}

void matrixMult(Matrix m1, Matrix m2, Matrix *out) {
    if (m1.cols != m2.rows) {
        printf("M1 & M2 must be of dimensions m x n and n x p\n");
        // exit(1);
        *out = (Matrix){0, 0, NULL};
    }
    Matrix temp;
    matrixCreate(m1.rows, m2.cols, &temp);
    for (int i = 0; i < m1.rows; i++) {
        for (int j = 0; j < m2.cols; j++) {
            float sum = 0;
            for (int k = 0; k < m1.cols; k++) {
                sum += m1.data[i * m1.cols + k] * m2.data[k * m2.cols + j];
            }
            temp.data[i * m2.cols + j] = sum;
        }
    }

    matrixCopy(temp, out);
    matrixFree(temp);
}

void matrixScalarMult(Matrix m, float k, Matrix *out) {
    Matrix temp;
    matrixCreate(m.rows, m.cols, &temp);
    for (int i = 0; i < m.rows * m.cols; i++) {
        temp.data[i] = m.data[i] * k;
    }

    matrixCopy(temp, out);
    matrixFree(temp);
}

void matrixTranspose(Matrix m, Matrix *out) {
    Matrix temp;
    matrixCreate(m.cols, m.rows, &temp);
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            temp.data[j * m.rows + i] = m.data[i * m.cols + j];
        }
    }

    matrixCopy(temp, out);
    matrixFree(temp);
}

void matrixInverse(Matrix m, Matrix *out) {
    if (m.rows != m.cols || m.rows == 0) {
        printf("Matrix must be square\n");
        // exit(1);
        *out = (Matrix){0, 0, NULL};
    }

    int n = m.rows;
    matrixIdentity(n, n, out);
    float *data = m.data;
    
    for (int i = 0; i < n; i++) {
        float pivot = data[i * n + i];
        if (pivot == 0) {
            printf("Matrix is not invertible\n");
            // exit(1);
            *out = (Matrix){0, 0, NULL};
        }

        for (int j = 0; j < n; j++) {
            data[i * n + j] /= pivot;
            out->data[i * n + j] /= pivot;
        }

        for (int k = 0; k < n; k++) {
            if (k == i) continue;
            float ratio = data[k * n + i];
            for (int j = 0; j < n; j++) {
                data[k * n + j] -= ratio * data[i * n + j];
                out->data[k * n + j] -= ratio * out->data[i * n + j];
            }
        }
    }
}

/*******************************************************/
/*********** VECTOR TO MATRIX OPERATIONS ***************/
/*******************************************************/
void matrixLoadVec3f(Matrix m, int col, Vec3f data) {
    // float data[3] = {data->x, data->y, data->z};
    m.data[0 * m.cols + col] = data.x;
    m.data[1 * m.cols + col] = data.y;
    m.data[2 * m.cols + col] = data.z;
}

void matrixLoadVec4f(Matrix m, int col, Vec4f data) {
    m.data[0 * m.cols + col] = data.x;
    m.data[1 * m.cols + col] = data.y;
    m.data[2 * m.cols + col] = data.z;
    m.data[3 * m.cols + col] = data.w;
}

Vec3f matrixGetVec3f(Matrix m, int col) {
    Vec3f v = {
        .x = m.data[0 * m.cols + col],
        .y = m.data[1 * m.cols + col],
        .z = m.data[2 * m.cols + col]
    };
    return v;
}

Vec4f matrixGetVec4f(Matrix m, int col) {
    Vec4f v = {
        .x = m.data[0 * m.cols + col],
        .y = m.data[1 * m.cols + col],
        .z = m.data[2 * m.cols + col],
        .w = m.data[3 * m.cols + col]
    };
    return v;
}

void matrixLoadColumn(Matrix m, int col, float *data) {
    for (int i = 0; i < m.rows; i++) {
        data[i] = m.data[i * m.cols + col];
    }
}


void matrixIdentity(int rows, int cols, Matrix *out) {
    if (out->data == NULL) matrixCreate(rows, cols, out);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            out->data[i * cols + j] = (i == j) ? 1 : 0;
        }
    }
}

void matrixCopy(Matrix m, Matrix *out) {
    if (out->data == NULL) matrixCreate(m.rows, m.cols, out);
    for (int i = 0; i < m.rows * m.cols; i++) {
        out->data[i] = m.data[i];
    }
}

void matrixPrint(Matrix m) {
    if (m.data == NULL) {
        printf("Matrix is empty\n");
        return;
    }

    for (int i = 0; i < m.rows; i++) {
        // // Change the color of the text to blue
        printf("\033[0;34m");
        printf("| ");
        // // Change the color of the text to green
        printf("\033[0;32m");
        for (int j = 0; j < m.cols; j++) {
            // print with 2 decimal places. Align the columns to the right
            // with a width of 8
            printf("%8.2f ", m.data[i * m.cols + j]);   
        }
        printf("\033[0;34m");
        printf(" |\n");
        printf("\n");
        
    }
    // Reset the color of the text
    printf("\033[0m");
}
void matrixFree(Matrix m) {
    // if (m.data == NULL) return;
    free(m.data);
}   


/*****************************/
/*       CLIENT EXAMPLE    */
/*****************************/

// void userInputExample(void) {
//     // The example will allow the user to enter as many matrices as they want with names from a to k. 
//     // The user will continue to enter matrices until they enter 'c' to continue.
//     // They will then be able to perform operations on the matrices they entered i.e.,
//     // add A B; multiply C D; transpose E; inverse F; print G; quit;
//     int numMatrices = 11;
//     Matrix matrices[numMatrices];
//     char name = 'a';
//     int index = 0;
//     char operation;
//     int count = 0;

//     while (1) {
//         // Clear the screen and go to the top left corner
//         printf("\033[2J\033[1;1H");
//         printf("Enter the number of rows followed by the columns for %c:\n", name);
//         int rows, cols;
//         scanf("%d %d", &rows, &cols);
//         // matrices[index] = 
//         matrixCreate(rows, cols, &matrices[index]);
//         // Enter the data for the matrix in a single line separated by spaces
//         printf("Enter the data for the matrix separated by spaces\n");
        
//         for (int i = 0; i < rows * cols; i++) {
//             float val;
//             scanf("%f", &val);
//             printf("%f\n", val);
//             matrices[index].data[i] = val;
//         }

//         printf("Enter 'c' to continue or '.' add another matrix\n");
//         char c;
//         scanf(" %c", &c);
//         count++;
//         // If the user presses enter, increment the name and index
//         if (c == 'c') break;

//         name++;
//         index++;
        
//     }

//     // print all the matrices
//     for (int i = 0; i < count; i++) {
//         if (matrices[i].data == NULL) break;
//         printf("Matrix %c\n", 'A' + i);
//         matrixPrint(matrices[i]);
//     }

//     Matrix result;
//     matrixCreate(0, 0, &result);
//     Matrix m1;
//     matrixCreate(0, 0, &m1);
//     Matrix m2;
//     matrixCreate(0, 0, &m2);
//     while (1) {
//         // Clear the screen and go to the top left corner
//         printf("Enter the operation you want to perform\n");
//         printf("Options: (a)dd (s)ubtract (m)ultiply (x)scalar mult (t)ranspose (i)nverse (p)rint (q)uit\n");
//         // Get the operation and the matrix or matrices to perform the operation on
//         // Example: add A B means add matrix A and matrix B

//         scanf(" %c", &operation);
//         if (operation == 'q') break;

//         if (operation == 'p') {
//             printf("Enter the matrix you want to print\n");
//             char matrixName;
//             scanf(" %c", &matrixName);
//             int idx = matrixName - 'a';
//             if (matrixName != 'r' && (0 <= idx && idx >= numMatrices)) {
//                 printf("Matrix %c does not exist\n", matrixName);
//                 continue;
//             }

//             if (matrixName == 'r') {
//                 matrixPrint(result);
//                 continue;
//             } else {
//                 matrixPrint(matrices[idx]);
//             }
//         }

//         if (operation == 'a' || operation == 's' || operation == 'm') {
//             printf("Enter the matrices you want to perform the operation on\n");
//             char matrixName1, matrixName2;
//             scanf(" %c %c", &matrixName1, &matrixName2);
//             int idx1 = matrixName1 - 'a';
//             int idx2 = matrixName2 - 'a';

//             if (matrixName1 != 'r' && (0 <= idx1 && idx1 >= numMatrices)) {
//                 printf("Matrix %c does not exist\n", matrixName1);
//                 continue;
//             }
//             if (matrixName2 != 'r' && (0 <= idx2 && idx2 >= numMatrices)) {
//                 printf("Matrix %c does not exist\n", matrixName2);
//                 continue;
//             }
//             // m1 = (matrixName1 == 'r') ? result : matrices[idx1];
//             // m2 = (matrixName2 == 'r') ? result : matrices[idx2];
//             m1 = (matrixName1 == 'r') ? matrixCopy(result) : matrixCopy(matrices[idx1]);
//             m2 = (matrixName2 == 'r') ? matrixCopy(result) : matrixCopy(matrices[idx2]);
//             if (operation == 'a') {
//                 result = matrixAdd(m1, m2);
//                 if (result.data == NULL) {
//                     matrixCreate(0, 0, &result);
//                     continue;
//                 }
                
//                 matrixPrint(result);
//             }

//             if (operation == 's') {
//                 result = matrixSub(m1, m2);
//                 if (result.data == NULL) {
//                     matrixCreate(0, 0, &result);
//                     continue;
//                 }
//                 matrixPrint(result);
//             }

//             if (operation == 'm') {
//                 result = matrixMult(m1, m2);
//                 if (result.data == NULL) {
//                     matrixCreate(0, 0, &result);
//                     continue;
//                 }
                
//                 matrixPrint(result);
//             }
            
//         }

//         if (operation == 'x') {
//             printf("Enter the matrix and the scalar you want to multiply\n");
//             char matrixName;
//             float scalar;
//             scanf(" %c %f", &matrixName, &scalar);
//             int idx = matrixName - 'a';


//             if (matrixName != 'r' && (0 <= idx && idx >= numMatrices)) {
//                 printf("Matrix %c does not exist\n", matrixName);
//                 continue;
//             }
            
//             m1 = (matrixName == 'r') ? matrixCopy(result) : matrixCopy(matrices[idx]);
//             result = matrixScalarMult(m1, scalar);
//             if (result.data == NULL) {
//                 matrixCreate(0, 0, &result);
//                 continue;
//             }
//             matrixPrint(result);
//         }

//         if (operation == 't') {
//             printf("Enter the matrix you want to transpose\n");
//             char matrixName;
//             scanf(" %c", &matrixName);
//             int idx = matrixName - 'a';

//             if (matrixName != 'r' && (0 <= idx && idx >= numMatrices)) {
//                 printf("Matrix %c does not exist\n", matrixName);
//                 continue;
//             }

//             m1 = (matrixName == 'r') ? matrixCopy(result) : matrixCopy(matrices[idx]);
//             result = matrixTranspose(m1);
//             if (result.data == NULL) {
//                 matrixCreate(0, 0, &result);
//                 continue;
//             }
//             matrixPrint(result);
//         }
        
//         if (operation == 'i') {
//             printf("Enter the matrix you want to invert\n");
//             char matrixName;
//             scanf(" %c", &matrixName);
//             int idx = matrixName - 'a';

//             if (matrixName != 'r' && (0 <= idx && idx >= numMatrices)) {
//                 printf("Matrix %c does not exist\n", matrixName);
//                 continue;
//             }

//             m1 = (matrixName == 'r') ? matrixCopy(result) : matrixCopy(matrices[idx]);

//             result = matrixInverse(m1);
//             if (result.data == NULL) {
//                 matrixCreate(0, 0, &result);
//                 continue;
//             }
//             matrixPrint(result);
//         }
//     }



//     printf("m1: %p\n", m1.data);
//     printf("m2: %p\n", m2.data);
//     matrixFree(m1);
//     matrixFree(m2);
    
//     // Free the matrices
//     printf("Freeing %d matrices\n", count);
//     for (int i = 0; i < count; i++) {
//         if (matrices[i].data == NULL) break;
//         matrixFree(matrices[i]);
//     }
//     matrixFree(result);
// }

// void matrixExample(void) {
//     userInputExample();
// }