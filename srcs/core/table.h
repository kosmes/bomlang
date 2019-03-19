//
// Created by jiney on 2019-03-19.
//

#ifndef BOM_TABLE_H
#define BOM_TABLE_H

#include "common.h"

typedef struct table_pair table_pair_t;
typedef struct table table_t;

struct table_pair {
    u16char *key;
    void *data;

    table_pair_t *next;
};

struct table {
    size_t len, cap;
    table_pair_t **pairs;
};

DLL_EXPORT void init_table(table_t *table);

DLL_EXPORT void final_table(table_t *table);

DLL_EXPORT table_pair_t *find_by_key(table_t *table, const u16char *key);

DLL_EXPORT table_pair_t *insert_value(table_t *table, const u16char *key, void *data);

#endif //BOM_TABLE_H
