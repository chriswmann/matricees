#ifndef _MAT_H
#define _MAT_H

#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t rows, cols;
  float *data;
} Mat;

Mat mat_init(uint32_t rows, uint32_t cols);
void mat_destroy(Mat *m);
void mat_print(Mat m);

#endif // !_MAT_H
