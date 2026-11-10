#include "mat.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

Mat mat_init(uint32_t rows, uint32_t cols) {
  if (cols != 0 && (size_t)rows > SIZE_MAX / cols)
    return (Mat){0};

  size_t count = (size_t)rows * cols;

  if (count > SIZE_MAX / sizeof(float))
    return (Mat){0};

  // Allocate at least one element so emtpy arrays can succeed too
  float *data = calloc(count ? count : 1, sizeof(*data));
  if (!data)
    return (Mat){0};

  return (Mat){
      .rows = rows,
      .cols = cols,
      .data = data,
  };
}

void mat_destroy(Mat *m) {
  free(m->data);
  *m = (Mat){0};
}

void mat_print(Mat m) {
  for (size_t i = 0; i < m.rows; i++) {
    for (size_t j = 0; j < m.cols; j++) {
      printf(" %f", m.data[i * m.cols + j]);
    }
    printf("\n");
  }
}
