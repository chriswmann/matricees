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
  MatStatusKind status_kind_a = mat_init(&a, 3, 2);
  MatStatusKind status_kind_b = mat_init(&b, 2, 3);
  MatStatusKind status_kind_c = mat_init(&c, 3, 3);
  MatStatus statuses[3];
  MatStatusArray mat_statuses =
      (MatStatusArray){.size = 0, .statuses = statuses};

  if (status_kind_a != MAT_OK) {
    MatStatus status = (MatStatus){
        .kind = status_kind_a, .operation = mat_status_string(status_kind_a)};
    mat_statuses.statuses[mat_statuses.size++] = status;
  }

  if (status_kind_b != MAT_OK) {
    MatStatus status = (MatStatus){
        .kind = status_kind_b, .operation = mat_status_string(status_kind_b)};
    mat_statuses.statuses[mat_statuses.size++] = status;
  }

  if (status_kind_c != MAT_OK) {
    MatStatus status = (MatStatus){
        .kind = status_kind_c, .operation = mat_status_string(status_kind_c)};
    mat_statuses.statuses[mat_statuses.size++] = status;
  }

  if (mat_statuses.size > 0)
    goto cleanup;

  const float a_values[] = {1, 2, 3, 4, 5, 6};
  const float b_values[] = {6, 5, 4, 3, 2, 1};

  memcpy(a.data, a_values, sizeof(a_values));
  memcpy(b.data, b_values, sizeof(b_values));

  MatStatusArray status_array = mat_mul(&a, &b, &c);
  for (size_t i = 0; i < status_array.size; i++) {
    MatStatus status = status_array.statuses[i];
    if (status.kind != MAT_OK) {
      MatStatus status = (MatStatus){
          .kind = status.kind, .operation = mat_status_string(status.kind)};
      mat_statuses.statuses[mat_statuses.size++] = status;
    }
  }
  if (status_array.size > 0)
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
