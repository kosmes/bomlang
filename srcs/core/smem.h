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

#define create(t, d) ((t *) _create(sizeof(t), d));

void *_create(size_t size, void (*dtor)(void *));

void _delete(void *ptr);

void GeneralDtor(void *ptr);

#endif //BOM_SMEM_H
