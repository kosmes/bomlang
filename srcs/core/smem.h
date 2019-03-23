//
// Created by jiney on 2019-03-23.
//

#ifndef BOM_SMEM_H
#define BOM_SMEM_H

#include "common.h"

struct meta {
    void (*dtor)(void *);
    void *ptr;
};

void *new(size_t size, void (*dtor)(void *));

void delete(void *ptr);

#endif //BOM_SMEM_H
