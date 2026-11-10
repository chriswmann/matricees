#include "mat.h"

#include <stdio.h>
#include <stdlib.h>

void mat_print(Mat m) {
  for (size_t i = 0; i < m.rows; i++) {
    for (size_t j = 0; j < m.cols; j++) {
      printf(" %f", m.data[i * m.cols + j]);
    }
    printf("\n");
  }
}
