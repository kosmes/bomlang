//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_BUF_H
#define BOM_BUF_H

#include "common.h"

typedef struct BufHdr {
    size_t len;
    size_t cap;
    char buf[0];
} BufHdr;

#define buf__hdr(b)     ((BufHdr*)((char*)b - offsetof(BufHdr, buf)))
#define buf__fits(b, n)  (buf_len(b) + (n) <= buf_cap(b))
#define buf__fit(b, n)   (buf__fits(b, n) ? 0 : \
        ((b) = buf__grow((b), buf_len(b) + (n), sizeof(*(b)))))

#define buf_len(b)      ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b)      ((b) ? buf__hdr(b)->cap : 0)
#define buf_push(b, x)  (buf__fit(b, 1), b[buf_len(b)] = (x), buf__hdr(b)->len++)
#define buf_pop(b, x)   ((x) = b[buf_len(b) - 1], buf__hdr(b)->len--)
#define buf_free(b)     ((b) ? free(buf__hdr(b)) : 0)

DLL_EXPORT void *buf__grow(const void *buf, size_t newlen, size_t elemsize);

#endif //BOM_BUF_H
