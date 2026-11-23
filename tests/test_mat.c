#include "mat/mat.h"
#include <criterion/criterion.h>

Test(mat_init, records_requested_dimensions) {
  Mat m = {0};
  MatStatusKind result = mat_init(&m, 2, 3);
  cr_expect_eq(result, MAT_OK);
  cr_expect_eq(m.rows, 2);
  cr_expect_eq(m.cols, 3);
  mat_destroy(&m);
}
