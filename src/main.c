#include "mat/mat.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  Mat a = {0};
  Mat b = {0};
  Mat c = {0};
  int exit_code = EXIT_FAILURE;
  MatStatusArray mat_statuses = mat_status_array_init();
  MatStatusKind status = mat_init(&a, 3, 2);
  if (status != MAT_OK) {
    append_status(&mat_statuses, status, mat_status_string(status));
  }
  status = mat_init(&b, 2, 3);
  if (status != MAT_OK) {
    append_status(&mat_statuses, status, mat_status_string(status));
  }
  status = mat_init(&c, 3, 3);
  if (status != MAT_OK) {
    append_status(&mat_statuses, status, mat_status_string(status));
  }
  if (status != MAT_OK) {
    append_status(&mat_statuses, status, mat_status_string(status));
  }

  if (mat_statuses.size > 0)
    goto cleanup;

  const float a_values[] = {1, 2, 3, 4, 5, 6};
  const float b_values[] = {6, 5, 4, 3, 2, 1};

  memcpy(a.data, a_values, sizeof(a_values));
  memcpy(b.data, b_values, sizeof(b_values));

  mat_mul(&a, &b, &c, &mat_statuses);
  if (mat_statuses.size > 0)
    goto cleanup;

  mat_print(&a);
  mat_print(&b);
  mat_print(&c);
  exit_code = EXIT_SUCCESS;

cleanup:
  for (size_t i = 0; i < mat_statuses.size; i++) {
    const char *message = mat_statuses.statuses[i].operation;
    fprintf(stderr, "%s\n", message);
  }
  mat_destroy(&a);
  mat_destroy(&b);
  mat_destroy(&c);
  return exit_code;
}
