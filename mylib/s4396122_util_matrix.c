/**
 * @file   s4396122_util_matrix.c
 * @author Daniel Fitzmaurice = 43961229
 * @date   120417
 * @brief  Library for adding Matrix functionality
 */
#include "s4396122_util_matrix.h"

/**
 * Creates an empty matrix of specified width and height
 * @param  width  Width of matrix
 * @param  height Height of matrix
 * @return        Pointer to Matrix object
 */
Matrix* s4396122_util_matrix_create(int width, int height) {
    if (width < 1 || height < 1) {
        return NULL;
    }
    Matrix *m = malloc(sizeof(Matrix));
    m->width = width;
    m->height = height;
    m->data = malloc(sizeof(int*) * height);
    for (int i = 0; i < height; i++) {
        m->data[i] = malloc(sizeof(int) * width);
    }
    return m;
}

/**
 * Gets the value at (x, y) from Matrix m
 * @param  m Matrix to get data from
 * @param  x X position to get the data from
 * @param  y Y position to get the data from
 * @return   The value located at position (x, y). If there is an error, -1 is
 * returned
 */
int s4396122_util_matrix_get(Matrix *m, int x, int y) {
    if (x < 0 || x >= m->width || y < 0 || y >= m->height) {
        return -1;
    }
    return m->data[y][x];
}

/**
 * Sets the value at position (x, y) of Matrix m
 * @param m   Matrix to set value to
 * @param x   X position to write to
 * @param y   Y position to write to
 * @param val value to set at position (x, y)
 */
void s4396122_util_matrix_set(Matrix *m, int x, int y, int val) {
    if (x < 0 || x >= m->width || y < 0 || y >= m->height) {
        return;
    }
    m->data[y][x] = val;
}

/**
 * Multiply two matrices together
 * @param  a Left Matrix
 * @param  b Right Matrix
 * @return   Resulting Matrix
 */
Matrix* s4396122_util_matrix_multi(Matrix *a, Matrix *b) {
    int width = a->height;
    int height = b->width;
    if (a->width != b->height) {
        debug_printf("Invalid Matrix sizes\n");
        return NULL;
    }
    Matrix *result = s4396122_util_matrix_create(width, height);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int sum = 0;
            for (int k = 0; k < a->width; k++) {
                sum += s4396122_util_matrix_get(b, i, k) * s4396122_util_matrix_get(a, k, j);
            }
            s4396122_util_matrix_set(result, j, i, sum);
        }
    }
    return result;
}

/**
 * Generate a matrix from a of predefined arrays
 * @param  width   Width of matrix
 * @param  height  Height of matrix
 * @param  VARARGS Arrays for the columns of the matrix
 * @return         The created matrix
 */
Matrix* s4396122_util_matrix_gen(int width, int height, ...) {
    va_list valist;
    va_start(valist, height);

    Matrix *m = s4396122_util_matrix_create(width, height);
    for (int i = 0; i < height; i++) {
        int *row = va_arg(valist, int[]);
        for (int j = 0; j < width; j++) {
            s4396122_util_matrix_set(m, j, i, row[j]);
        }
    }

    va_end(valist);
    return m;
}

/**
 * Prints the matrix to serial for debugging
 * @param m Matrix to be printed
 */
void s4396122_util_matrix_print(Matrix *m) {
    for (int i = 0; i < m->height; i++) {
        for (int j = 0; j < m->width; j++) {
            debug_printf("%d ", s4396122_util_matrix_get(m, j, i));
        }
        debug_printf("\n");
    }
}

/**
 * Executes a function over every cell of the matrix and sets the return value
 * to the new value of the cell
 * @param m        Matrix to be executed on
 * @param function Function to be execute over the cells
 */
void s4396122_util_matrix_exec(Matrix *m, int (*function)(int)) {
    for (int i = 0; i < m->height; i++) {
        for (int j = 0; j < m->width; j++) {
            int processedInt = function(s4396122_util_matrix_get(m, j, i));
            s4396122_util_matrix_set(m, j, i, processedInt);
        }
    }
}

/**
 * Frees a Matrix from memory
 * @param m Matrix to be freed
 */
void s4396122_util_matrix_free(Matrix *m) {
    for (int i = 0; i < m->height; i++) {
        free(m->data[i]);
    }
    free(m->data);
    free(m);
}
