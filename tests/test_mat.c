#include "criterion/assert.h"
#include "criterion/internal/assert.h"
#include "mat/mat.h"
#include <criterion/criterion.h>
#include <string.h>

Test(mat_init, records_requested_dimensions) {
  Mat m = {0};
  MatStatusKind result = mat_init(&m, 2, 3);
  cr_expect_eq(result, MAT_OK);
  cr_expect_eq(m.rows, 2);
  cr_expect_eq(m.cols, 3);
  mat_destroy(&m);
}

Test(mat_destroy, resets_matrix_handle) {
  Mat m = {0};
  MatStatusKind result = mat_init(&m, 2, 3);
  cr_expect_eq(result, MAT_OK);
  cr_assert_not_null(m.data);
  mat_destroy(&m);
  cr_assert_null(m.data);
  cr_assert_eq(m.rows, 0);
  cr_assert_eq(m.cols, 0);
}

Test(mat_mul, gives_expected_result_for_conformable_multiplication) {
  Mat out = {0};
  MatStatusKind out_init_status = mat_init(&out, 2, 2);
  cr_assert_eq(out_init_status, MAT_OK);
  Mat a = {0};
  MatStatusKind a_init_status = mat_init(&a, 2, 2);
  cr_assert_eq(a_init_status, MAT_OK);
  Mat b = {0};
  MatStatusKind b_init_status = mat_init(&b, 2, 2);
  cr_assert_eq(b_init_status, MAT_OK);

  const float a_data[] = {2, 3};
  const float b_data[] = {6, 4};

  memcpy(a.data, a_data, sizeof(a_data));
  memcpy(b.data, b_data, sizeof(b_data));

  MatStatusArray mat_statuses = mat_status_array_init();
  mat_mul(&a, &b, &out, &mat_statuses);

  cr_assert_eq(mat_statuses.statuses[0].kind, MAT_OK);
  mat_destroy(&out);
  mat_destroy(&a);
  mat_destroy(&b);
}
