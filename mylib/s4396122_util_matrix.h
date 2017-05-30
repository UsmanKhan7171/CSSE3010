/**
 * @file s4396122_util_matrix.h
 * @brief Library for adding Matrix functionality
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef UTIL_MATRIX_H
#define UTIL_MATRIX_H

#include <stdlib.h>
#include <stdarg.h>

/**
 * @brief Structure containing all the information required for a Matrix
 */
typedef struct {
    int width;  //!< Width of the Matrix
    int height; //!< Height of the Matrix
    int **data; //!< Data contained within the Matrix
} Matrix;

Matrix* s4396122_util_matrix_create(int width, int height);
int s4396122_util_matrix_get(Matrix *m, int x, int y);
void s4396122_util_matrix_set(Matrix *m, int x, int y, int val);
Matrix* s4396122_util_matrix_multi(Matrix *a, Matrix *b);
Matrix* s4396122_util_matrix_gen(int width, int height, ...);
void s4396122_util_matrix_print(Matrix *m);
void s4396122_util_matrix_exec(Matrix *m, int (*function)(int));
void s4396122_util_matrix_free(Matrix *m);

#endif
