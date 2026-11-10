#include "mat/mat.h"
#include <stdint.h>

int main(void) {
  Mat a = {.rows = 2, .cols = 3, .data = (float[6]){0}};
  mat_print(a);
  return 0;
}
