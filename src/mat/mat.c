#include "mat.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static MatStatusKind mat_element_count(uint32_t rows, uint32_t cols,
                                       size_t *count) {
/*
 * Check before converting to size_t, including on platforms
 * where size_t could be narrower than uint32_t.
 */
#if SIZE_MAX < UINT32_MAX
  if ((uintmax_t)rows > (uintmax_t)SIZE_MAX ||
      (uintmax_t)cols > (uintmax_t)SIZE_MAX) {
    return MAT_ERR_OVERFLOW;
  }
#endif

  /* Check we can handle the element count. */
  size_t r = (size_t)rows;
  size_t c = (size_t)cols;
  if (c != 0 && r > SIZE_MAX / c) {
    return MAT_ERR_OVERFLOW;
  }

  /* Check we can handle the allocation itself. */
  size_t n = r * c;

  if (n > SIZE_MAX / sizeof(float)) {
    return MAT_ERR_OVERFLOW;
  }

  *count = n;
  return MAT_OK;
}

MatStatusKind mat_init(Mat *out, uint32_t rows, uint32_t cols) {
  if (out == NULL) {
    return MAT_ERR_ARGUMENT;
  }

  /* Don't silently overwrite an owning handle. */
  if (out->data != NULL || out->rows != 0 || out->cols != 0) {
    return MAT_ERR_ARGUMENT;
  }

  size_t count;
  MatStatusKind status = mat_element_count(rows, cols, &count);

  if (status != MAT_OK) {
    return status;
  }

  /* Give successful empty matrices a non-NULL allocation too. */

  size_t allocated_count = count != 0 ? count : 1;
  /*
   * Assume IEEE 754 floats, so we can use calloc and skip
   * explicit initialisation.
   */
  float *data = calloc(allocated_count, sizeof(*data));

  if (data == NULL) {
    return MAT_ERR_ALLOCATION;
  }
  *out = (Mat){.rows = rows, .cols = cols, .data = data};
  return MAT_OK;
}

void mat_destroy(Mat *m) {
  free(m->data);
  *m = (Mat){0};
}

MatStatusArray mat_mul(Mat *a, Mat *b, Mat *out) {
  MatStatusArray mat_statuses = {0};

  /*
   * Validate each pointer before accessing its data field.
   */
  if (a == NULL)
    append_status(&mat_statuses, MAT_ERR_ALLOCATION,
                  "mat_mul a pointer is NULL");

  if (a->data == NULL)
    append_status(&mat_statuses, MAT_ERR_ARGUMENT,
                  "mat_mul a data pointer is NULL");

  if (b == NULL)
    append_status(&mat_statuses, MAT_ERR_ARGUMENT, "mat_mul b pointer is NULL");

  if (b->data == NULL)
    append_status(&mat_statuses, MAT_ERR_ARGUMENT,
                  "mat_mul b data pointer is NULL");

  if (out == NULL)
    append_status(&mat_statuses, MAT_ERR_ARGUMENT,
                  "mat_mul out pointer is NULL");

  if (out->data == NULL)
    append_status(&mat_statuses, MAT_ERR_ARGUMENT,
                  "mat_mul out data pointer is NULL");

  if (mat_statuses.size > 0)
    return mat_statuses;

  if (a->cols != b->rows)
    append_status(&mat_statuses, MAT_ERR_DIMENSIONS,
                  "mat_mul a's columns must equal b's rows");

  if (out->rows != a->rows)
    append_status(&mat_statuses, MAT_ERR_DIMENSIONS,
                  "mat_mul out's rows must equal a's rows");

  if (out->cols != b->cols)
    append_status(&mat_statuses, MAT_ERR_DIMENSIONS,
                  "mat_mul out's cols must equal b's cols");

  if (mat_statuses.size > 0)
    return mat_statuses;

  size_t count = 0;

  MatStatusKind status = mat_element_count(a->rows, a->cols, &count);
  if (status != MAT_OK)
    append_status(&mat_statuses, status, "mat_mul's a's size overflows");

  status = mat_element_count(b->rows, b->cols, &count);
  if (status != MAT_OK)
    append_status(&mat_statuses, status, "mat_mul's b's size overflows");

  status = mat_element_count(out->rows, out->cols, &count);
  if (status != MAT_OK)
    append_status(&mat_statuses, status, "mat_mul's out's size overflows");

  if (mat_statuses.size > 0)
    return mat_statuses;

  /* No output elements means no allocation or copying is needed. */
  if (count == 0)
    return mat_statuses;

  float *result = malloc(count * sizeof(*result));
  if (result == NULL) {
    append_status(&mat_statuses, MAT_ERR_ALLOCATION,
                  "mat_mul_could not allocate the result buffer");
    return mat_statuses;
  }

  size_t rows = (size_t)a->rows;
  size_t inner = (size_t)a->cols;
  size_t cols = (size_t)b->cols;

  for (size_t i = 0; i < rows; ++i) {
    for (size_t j = 0; j < cols; ++j) {
      float sum = 0.0f;

      for (size_t k = 0; k < inner; ++k) {
        float product = a->data[i * inner + k] * b->data[k * cols + j];

        if (!isfinite(product)) {
          append_status(&mat_statuses, MAT_ERR_NONFINITE,
                        "mat_mul_product is not finite");
          free(result);
          return mat_statuses;
        }

        sum += product;

        if (!isfinite(sum)) {
          append_status(&mat_statuses, MAT_ERR_NONFINITE,
                        "mat_mul sum is not finite");
          free(result);
          return mat_statuses;
        }
      }

      size_t out_ix = i * cols + j;
      result[out_ix] = sum;
    }
  }

  /* Commit the result now all the calculations have been completed
   * successfully.
   */
  memcpy(out->data, result, count * sizeof(*result));
  free(result);

  return mat_statuses;
}

void mat_print(Mat *m) {
  for (size_t i = 0; i < m->rows; i++) {
    for (size_t j = 0; j < m->cols; j++) {
      printf(" %f", m->data[i * m->cols + j]);
    }
    printf("\n");
  }
  printf("\n");
}

void append_status(MatStatusArray *arr, MatStatusKind kind,
                   const char *operation) {
  arr->statuses[arr->size++] = (MatStatus){
      .kind = kind,
      .operation = operation,
  };
}

const char *mat_status_string(MatStatusKind s) {
  switch (s) {
  case MAT_OK:
    return "OK";
  case MAT_ERR_ALLOCATION:
    return "Matrix allocation error";
  case MAT_ERR_ARGUMENT:
    return "Argument error";
  case MAT_ERR_DIMENSIONS:
    return "Matrix dimensions error";
  case MAT_ERR_NONFINITE:
    return "Non-finite error";
  case MAT_ERR_OVERFLOW:
    return "Overflow error";
  };
}
