#include "mat/mat.h"
#include <stdint.h>

int main(void) {
  Mat a = mat_init(3, 2);
  a.data[0] = 4.42;
  a.data[5] = (float)2;
  mat_print(a);
  mat_destroy(&a);
  return 0;
}
