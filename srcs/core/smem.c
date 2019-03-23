//
// Created by jiney on 2019-03-23.
//

#include "smem.h"

static struct meta *getMeta(void *ptr) {
    return ptr - sizeof (struct meta);
}

void *new(size_t size, void (*dtor)(void *)) {
    struct meta *meta = malloc(sizeof (struct meta) + size);
    *meta = (struct meta) {
            .dtor = dtor,
            .ptr  = meta + 1
    };
    return meta->ptr;
}

void delete(void *ptr) {
    if (ptr == NULL)
        return;
    struct meta *meta = getMeta(ptr);
    assert(ptr == meta->ptr);
    meta->dtor(ptr);
    free(meta);
}