#include "s4396122_util_matrix.h"

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

int s4396122_util_matrix_get(Matrix *m, int x, int y) {
    if (x < 0 || x >= m->width || y < 0 || y >= m->height) {
        return -1;
    }
    return m->data[y][x];
}

void s4396122_util_matrix_set(Matrix *m, int x, int y, int val) {
    if (x < 0 || x >= m->width || y < 0 || y >= m->height) {
        return;
    }
    m->data[y][x] = val;
}

Matrix* s4396122_util_matrix_multi(Matrix *a, Matrix *b) {
    int width = a->height;
    int height = b->width;
    if (a->width != b->height) {
        debug_printf("Invalid Matrix sizes\n");
        return NULL;
    }
    Matrix *result = s4396122_util_matrix_create(width, height);
    debug_printf("Creating matrix: %d %d\n", width, height);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int sum = 0;
            for (int k = 0; k < a->width; k++) {
                sum += s4396122_util_matrix_get(b, i, k) * s4396122_util_matrix_get(a, k, j);
                // debug_printf("%d * %d, ", s4396122_util_matrix_get(b, i, k), s4396122_util_matrix_get(a, k, j));
            }
            // debug_printf("\n");
            s4396122_util_matrix_set(result, j, i, sum);
        }
    }
    return result;
}

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

void s4396122_util_matrix_print(Matrix *m) {
    for (int i = 0; i < m->height; i++) {
        for (int j = 0; j < m->width; j++) {
            debug_printf("%d ", s4396122_util_matrix_get(m, j, i));
        }
        debug_printf("\n");
    }
}

void s4396122_util_matrix_exec(Matrix *m, int (*function)(int)) {
    for (int i = 0; i < m->height; i++) {
        for (int j = 0; j < m->width; j++) {
            int processedInt = function(s4396122_util_matrix_get(m, j, i));
            s4396122_util_matrix_set(m, j, i, processedInt);
        }
    }
}

void s4396122_util_matrix_free(Matrix *m) {
    for (int i = 0; i < m->height; i++) {
        free(m->data[i]);
    }
    free(m->data);
    free(m);
}
