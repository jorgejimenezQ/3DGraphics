#include "utils.h"

void sort(Triangle *triangles) {
    int n = array_length(triangles);

    // Create the auxilary array
    Triangle *aux = malloc(n * sizeof(Triangle));
    sortHelper(triangles, aux, 0, n - 1);
    free(aux);
}

void sortHelper(Triangle *triangles, Triangle *aux, int lo, int hi) {
    if (hi <= lo) return;
    int mid = lo + (hi - lo) / 2;
    sortHelper(triangles, aux, lo, mid);
    sortHelper(triangles, aux, mid + 1, hi);
    merge(triangles, aux, lo, mid, hi);
}
// Sort the halfs. First half [lo, mid] and second half [mid + 1, hi]
void merge(Triangle *triangles, Triangle *aux, int lo, int mid, int hi) {
    // TODO: assert the halfs are sorted
    for (int l = lo; l <= hi; l++) aux[l] = triangles[l];

    int i = lo, j = mid + 1;
    for (int k = lo; k <= hi; k++) {
        if (i > mid) {
            triangles[k] = aux[j++];
        }
        else if (j > hi) {
            triangles[k] = aux[i++];
        } 
        else if (aux[j].avgDepth < aux[i].avgDepth) {
            triangles[k] = aux[j++];
        }
        else {
            triangles[k] = aux[i++];
        }
    }
}

// Assert function
void assert(int condition, const char *message) {
    if (!condition) {
        printf("Assertion failed with message: %s\n", message);
        exit(1);
    }
}

// CLIENT EXAMPLE

void mergeExample(int n) {
    Triangle *triangles = NULL;

    for (int i = 0; i < n; i++) {
        Triangle triangle = {
            .avgDepth = rand() % 1000
        };
        array_push(triangles, triangle);
    }

    // Sort the array
    sort(triangles);

    // Assert the array is sorted
    for (int i = 0; i < n - 1; i++) {
        assert(triangles[i].avgDepth <= triangles[i + 1].avgDepth, "Array is not sorted");
    }
    array_free(triangles);

    printf("Merge sort example passed\n");
}