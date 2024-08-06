#ifndef UTILS_H
#define UTILS_H

// Include any necessary libraries or headers
#include "../geometry/geometry.h"
#include "../array.h"
#include <stdio.h>
#include <stdlib.h>

// Declare any global constants or macros

// Declare any function prototypes or class declarations
/**
 * @details Sorts the faces by their average depth
 */
void sort(Triangle *triangles);
void sortHelper(Triangle *triangles, Triangle *aux, int lo, int hi);
void merge(Triangle *triangles, Triangle *aux, int lo, int mid, int hi);
void assert(int condition, const char *message);

/****************************/
/*        CLIENT EXAMPLES   */
/****************************/
void mergeExample(int n);

#endif // UTILS_H