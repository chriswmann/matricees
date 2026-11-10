#ifndef _MAT_H
#define _MAT_H

#include <stdint.h>

typedef struct {
  float *data;
  uint32_t rows, cols;
} Mat;

void mat_print(Mat m);

#endif // !_MAT_H
