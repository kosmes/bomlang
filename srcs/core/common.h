#ifndef COMMON_H
#define COMMON_H

#include "platform.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <wchar.h>
#include <wctype.h>
#include <memory.h>

#include "unicode.h"

#define MAX(a, b) (a >= b ? a : b)
#define MIN(a, b) (a <= b ? a : b)

typedef union converter converter_t;

union converter {
    short as_short;
    double asDouble;
    int as_integer;
    u16char as_char;
    size_t as_size;
    unsigned char as_bytes[8];
};

#endif /* ifndef COMMON_H */
