#ifndef MATRICEES_MAT_H
#define MATRICEES_MAT_H

#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t rows, cols;
  float *data;
} Mat;

typedef enum {
  MAT_OK = 0,
  MAT_ERR_ARGUMENT,
  MAT_ERR_DIMENSIONS,
  MAT_ERR_NONFINITE,
  MAT_ERR_ALLOCATION,
  MAT_ERR_OVERFLOW,
} MatStatusKind;

typedef struct {
  MatStatusKind kind;
  const char *operation;
} MatStatus;

#define MAT_MAX_ERRORS 6

typedef struct {
  size_t size;
  size_t capacity;
  MatStatus statuses[MAT_MAX_ERRORS];
} MatStatusArray;

/* Initialise a MatStatusArray. */
MatStatusArray mat_status_array_init(void);

/*
 * Initialise an empty handle, normally declared as
 * Mat m = {0}. On failure, *out is unchanged. Destroy
 * an existing allocation before reinitialising its
 * handle.
 */
MatStatusKind mat_init(Mat *out, uint32_t rows, uint32_t cols);

/* Accepts NULL. Resets the handle after releasing its buffer. */
void mat_destroy(Mat *m);

/*
 * Inputs and output must be successfully initialised
 * matrices. out must have dimensions a->rows by b->cols.
 * Output may overlap either input.
 * On failure, out and its buffer are unchanged.
 */
void mat_mul(Mat *a, Mat *b, Mat *out, MatStatusArray *statuses);

/* Requires a valid, initialised matrix. */
void mat_print(Mat *m);

/* Append a MatStatus to a MatStatusArray. */
void append_status(MatStatusArray *arr, MatStatusKind kind,
                   const char *operation);

/* Convert a MatStatus to a readable string. */
const char *mat_status_string(MatStatusKind s);

#endif // !MATRICEES_MAT_H
