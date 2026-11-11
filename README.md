# matricees

A small matrix library written in C, built as a learning project.

Matrices store `float` values in a contiguous, row-major buffer. The library
currently supports allocation, multiplication, printing and cleanup.

## Build and run

Requires a C17 compiler and GNU Make. The implementation assumes IEEE 754 floats.

```sh
make
make run
```

`make` builds `bin/main`; `make run` runs the example in
[`src/main.c`](src/main.c).

For automatic rebuilding when source files change, install `entr` and run:

```sh
make hot
```

Remove build output with:

```sh
make clean
```

## Example

```c
#include "mat/mat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    Mat a = {0};
    Mat b = {0};
    Mat out = {0};
    int result = EXIT_FAILURE;
    MatStatusArray errors = mat_status_array_init();

    if (mat_init(&a, 2, 2) != MAT_OK ||
        mat_init(&b, 2, 2) != MAT_OK ||
        mat_init(&out, 2, 2) != MAT_OK) {
        fprintf(stderr, "Could not initialise matrices\n");
        goto cleanup;
    }

    const float a_values[] = {1, 2, 3, 4};
    const float b_values[] = {5, 6, 7, 8};

    memcpy(a.data, a_values, sizeof(a_values));
    memcpy(b.data, b_values, sizeof(b_values));

    mat_mul(&a, &b, &out, &errors);
    if (errors.size != 0) {
        for (size_t i = 0; i < errors.size; ++i) {
            fprintf(stderr, "%s\n", errors.statuses[i].operation);
        }
        goto cleanup;
    }

    mat_print(&out);
    result = EXIT_SUCCESS;

cleanup:
    mat_destroy(&out);
    mat_destroy(&b);
    mat_destroy(&a);
    return result;
}
```

Output:

```text
 19.000000 22.000000
 43.000000 50.000000
```

To build this as a separate file named `example.c` in the project root:

```sh
cc -std=c17 -Wall -Wextra -Isrc example.c src/mat/mat.c -o example
./example
```

Elements are stored in row-major order:

```c
matrix.data[row * matrix.cols + column]
```

## Multiplication and errors

For an `m × n` matrix multiplied by an `n × p` matrix, allocate an `m × p`
output matrix before calling `mat_mul`.

`mat_init` returns a `MatStatusKind`: `MAT_OK` indicates success. Use
`mat_status_string` to obtain a readable description of a status kind.

`mat_mul` takes a non-null pointer to an empty `MatStatusArray` created with
`mat_status_array_init()`. A size of zero after the call indicates success;
otherwise, inspect each entry's `kind` and `operation`. Reinitialise the status
array before reusing it for another call, and leave its capacity unchanged.

Multiplication checks arguments, dimensions and allocation sizes, and reports
allocation failures and nonfinite products or running sums. It calculates into
a temporary buffer before copying to the output, so the output may overlap an
input and remains unchanged on failure. This requires an additional allocation
the size of the output.

Zero-sized dimensions are supported. A product with a zero inner dimension
writes zeros to its output. Nonfinite checks do not detect ordinary rounding
error or underflow.

## Licence

Licensed under the [Apache License 2.0](LICENSE).
