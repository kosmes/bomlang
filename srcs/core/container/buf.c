//
// Created by jiney on 2019-03-17.
//

#include "buf.h"

void *buf__grow(const void *buf, size_t newlen, size_t elemsize) {
    size_t newcap = MAX(1 + 2 * buf_cap(buf), newlen);
    assert(newlen <= newcap);
    size_t newsize = offsetof(BufHdr, buf) + newcap * elemsize;
    BufHdr *newhdr;
    if (buf) {
        newhdr = realloc(buf__hdr(buf), newsize);
    } else {
        newhdr = malloc(newsize);
        newhdr->len = 0;
    }

    newhdr->cap = newcap;
    return newhdr->buf;
}