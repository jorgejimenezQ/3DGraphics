#include "matrix.h"

Matrix matrixCreate(int rows, int cols) {
    Matrix m;
    m.rows = rows;
    m.cols = cols;
    m.data = (float *)malloc(rows * cols * sizeof(float));
    return m;
}

Matrix matrixCreateWithData(int rows, int cols, float *data) {
    // Check the number of arguments
    if (data == NULL) {
        printf("Data cannot be NULL\n");
        return (Matrix){0, 0, NULL};
    }

    Matrix m = matrixCreate(rows, cols);
    for (int i = 0; i < rows * cols; i++) {
        m.data[i] = data[i];
    }
    return m;
}

Matrix matrixAdd(Matrix m1, Matrix m2) {
    if (m1.rows != m2.rows || m1.cols != m2.cols) {
        printf("Matrix dimensions do not match\n");
    }

    Matrix result = matrixCreate(m1.rows, m1.cols);
    for (int i = 0; i < m1.rows * m1.cols; i++) {
        result.data[i] = m1.data[i] + m2.data[i];
    }
    return result;
}

Matrix matrixSub(Matrix m1, Matrix m2) {
    if (m1.rows != m2.rows || m1.cols != m2.cols) {
        printf("Matrix dimensions do not match\n");
        // exit(1);
        return (Matrix){0, 0, NULL};
    }

    Matrix result = matrixCreate(m1.rows, m1.cols);
    for (int i = 0; i < m1.rows * m1.cols; i++) {
        result.data[i] = m1.data[i] - m2.data[i];
    }
    return result;
}

Matrix matrixMult(Matrix m1, Matrix m2) {
    if (m1.cols != m2.rows) {
        printf("M1 & M2 must be of dimensions m x n and n x p\n");
        // exit(1);
        return (Matrix){0, 0, NULL};
    }

    Matrix result = matrixCreate(m1.rows, m2.cols);
    for (int i = 0; i < m1.rows; i++) {
        for (int j = 0; j < m2.cols; j++) {
            float sum = 0;
            for (int k = 0; k < m1.cols; k++) {
                sum += m1.data[i * m1.cols + k] * m2.data[k * m2.cols + j];
            }
            result.data[i * m2.cols + j] = sum;
        }
    }
    return result;
}

Matrix matrixScalarMult(Matrix m, float k) {
    Matrix result = matrixCreate(m.rows, m.cols);
    for (int i = 0; i < m.rows * m.cols; i++) {
        result.data[i] = m.data[i] * k;
    }
    return result;
}

Matrix matrixTranspose(Matrix m) {
    Matrix result = matrixCreate(m.cols, m.rows);
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            result.data[j * m.rows + i] = m.data[i * m.cols + j];
        }
    }
    return result;
}

Matrix matrixInverse(Matrix m) {
    if (m.rows != m.cols || m.rows == 0) {
        printf("Matrix must be square\n");
        // exit(1);
        return (Matrix){0, 0, NULL};
    }

    int n = m.rows;
    Matrix result = matrixIdentity(n, n);
    float *data = m.data;
    
    for (int i = 0; i < n; i++) {
        float pivot = data[i * n + i];
        if (pivot == 0) {
            printf("Matrix is not invertible\n");
            // exit(1);
            return (Matrix){0, 0, NULL};
        }

        for (int j = 0; j < n; j++) {
            data[i * n + j] /= pivot;
            result.data[i * n + j] /= pivot;
        }

        for (int k = 0; k < n; k++) {
            if (k == i) continue;
            float ratio = data[k * n + i];
            for (int j = 0; j < n; j++) {
                data[k * n + j] -= ratio * data[i * n + j];
                result.data[k * n + j] -= ratio * result.data[i * n + j];
            }
        }
    }
    return result;
}

Matrix matrixIdentity(int rows, int cols) {
    Matrix m = matrixCreate(rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            m.data[i * cols + j] = (i == j) ? 1 : 0;
        }
    }
    return m;
}

Matrix matrixCopy(Matrix m) {
    Matrix copy = matrixCreate(m.rows, m.cols);
    for (int i = 0; i < m.rows * m.cols; i++) {
        copy.data[i] = m.data[i];
    }
    return copy;
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
    if (m.data == NULL) return;
    printf("Freeing matrix: %p\n", m.data);
    free(m.data);
}   


/*****************************/
/*       CLIENT EXAMPLE    */
/*****************************/

void userInputExample(void) {
    // The example will allow the user to enter as many matrices as they want with names from a to k. 
    // The user will continue to enter matrices until they enter 'c' to continue.
    // They will then be able to perform operations on the matrices they entered i.e.,
    // add A B; multiply C D; transpose E; inverse F; print G; quit;
    int numMatrices = 11;
    Matrix matrices[numMatrices];
    char name = 'a';
    int index = 0;
    char operation;
    int count = 0;

    while (1) {
        // Clear the screen and go to the top left corner
        printf("\033[2J\033[1;1H");
        printf("Enter the number of rows followed by the columns for %c:\n", name);
        int rows, cols;
        scanf("%d %d", &rows, &cols);
        matrices[index] = matrixCreate(rows, cols);
        // Enter the data for the matrix in a single line separated by spaces
        printf("Enter the data for the matrix separated by spaces\n");
        
        for (int i = 0; i < rows * cols; i++) {
            float val;
            scanf("%f", &val);
            printf("%f\n", val);
            matrices[index].data[i] = val;
        }

        printf("Enter 'c' to continue or '.' add another matrix\n");
        char c;
        scanf(" %c", &c);
        count++;
        // If the user presses enter, increment the name and index
        if (c == 'c') break;

        name++;
        index++;
        
    }

    // print all the matrices
    for (int i = 0; i < count; i++) {
        if (matrices[i].data == NULL) break;
        printf("Matrix %c\n", 'A' + i);
        matrixPrint(matrices[i]);
    }

    Matrix result = matrixCreate(0, 0);
    Matrix m1 = matrixCreate(0, 0);
    Matrix m2 = matrixCreate(0, 0);
    while (1) {
        // Clear the screen and go to the top left corner
        printf("Enter the operation you want to perform\n");
        printf("Options: (a)dd (s)ubtract (m)ultiply (x)scalar mult (t)ranspose (i)nverse (p)rint (q)uit\n");
        // Get the operation and the matrix or matrices to perform the operation on
        // Example: add A B means add matrix A and matrix B

        scanf(" %c", &operation);
        if (operation == 'q') break;

        if (operation == 'p') {
            printf("Enter the matrix you want to print\n");
            char matrixName;
            scanf(" %c", &matrixName);
            int idx = matrixName - 'a';
            if (matrixName != 'r' && (0 <= idx && idx >= numMatrices)) {
                printf("Matrix %c does not exist\n", matrixName);
                continue;
            }

            if (matrixName == 'r') {
                matrixPrint(result);
                continue;
            } else {
                matrixPrint(matrices[idx]);
            }
        }

        if (operation == 'a' || operation == 's' || operation == 'm') {
            printf("Enter the matrices you want to perform the operation on\n");
            char matrixName1, matrixName2;
            scanf(" %c %c", &matrixName1, &matrixName2);
            int idx1 = matrixName1 - 'a';
            int idx2 = matrixName2 - 'a';

            if (matrixName1 != 'r' && (0 <= idx1 && idx1 >= numMatrices)) {
                printf("Matrix %c does not exist\n", matrixName1);
                continue;
            }
            if (matrixName2 != 'r' && (0 <= idx2 && idx2 >= numMatrices)) {
                printf("Matrix %c does not exist\n", matrixName2);
                continue;
            }
            // m1 = (matrixName1 == 'r') ? result : matrices[idx1];
            // m2 = (matrixName2 == 'r') ? result : matrices[idx2];
            m1 = (matrixName1 == 'r') ? matrixCopy(result) : matrixCopy(matrices[idx1]);
            m2 = (matrixName2 == 'r') ? matrixCopy(result) : matrixCopy(matrices[idx2]);
            if (operation == 'a') {
                result = matrixAdd(m1, m2);
                if (result.data == NULL) {
                    result = matrixCreate(0, 0);
                    continue;
                }
                
                matrixPrint(result);
            }

            if (operation == 's') {
                result = matrixSub(m1, m2);
                if (result.data == NULL) {
                    result = matrixCreate(0, 0);
                    continue;
                }
                matrixPrint(result);
            }

            if (operation == 'm') {
                result = matrixMult(m1, m2);
                if (result.data == NULL) {
                    result = matrixCreate(0, 0);
                    continue;
                }
                
                matrixPrint(result);
            }
            
        }

        if (operation == 'x') {
            printf("Enter the matrix and the scalar you want to multiply\n");
            char matrixName;
            float scalar;
            scanf(" %c %f", &matrixName, &scalar);
            int idx = matrixName - 'a';


            if (matrixName != 'r' && (0 <= idx && idx >= numMatrices)) {
                printf("Matrix %c does not exist\n", matrixName);
                continue;
            }
            
            m1 = (matrixName == 'r') ? matrixCopy(result) : matrixCopy(matrices[idx]);
            result = matrixScalarMult(m1, scalar);
            if (result.data == NULL) {
                result = matrixCreate(0, 0);
                continue;
            }
            matrixPrint(result);
        }

        if (operation == 't') {
            printf("Enter the matrix you want to transpose\n");
            char matrixName;
            scanf(" %c", &matrixName);
            int idx = matrixName - 'a';

            if (matrixName != 'r' && (0 <= idx && idx >= numMatrices)) {
                printf("Matrix %c does not exist\n", matrixName);
                continue;
            }

            m1 = (matrixName == 'r') ? matrixCopy(result) : matrixCopy(matrices[idx]);
            result = matrixTranspose(m1);
            if (result.data == NULL) {
                result = matrixCreate(0, 0);
                continue;
            }
            matrixPrint(result);
        }
        
        if (operation == 'i') {
            printf("Enter the matrix you want to invert\n");
            char matrixName;
            scanf(" %c", &matrixName);
            int idx = matrixName - 'a';

            if (matrixName != 'r' && (0 <= idx && idx >= numMatrices)) {
                printf("Matrix %c does not exist\n", matrixName);
                continue;
            }

            m1 = (matrixName == 'r') ? matrixCopy(result) : matrixCopy(matrices[idx]);

            result = matrixInverse(m1);
            if (result.data == NULL) {
                result = matrixCreate(0, 0);
                continue;
            }
            matrixPrint(result);
        }
    }



    printf("m1: %p\n", m1.data);
    printf("m2: %p\n", m2.data);
    matrixFree(m1);
    matrixFree(m2);
    
    // Free the matrices
    printf("Freeing %d matrices\n", count);
    for (int i = 0; i < count; i++) {
        if (matrices[i].data == NULL) break;
        matrixFree(matrices[i]);
    }
    matrixFree(result);
}

void matrixExample(void) {
    userInputExample();
}