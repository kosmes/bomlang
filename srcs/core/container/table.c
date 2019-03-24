//
// Created by jiney on 2019-03-19.
//

#include "table.h"
#include "error.h"

static size_t hash_string(const u16char *key) {
    size_t h = 0;
    size_t off = 0;
    size_t len = wcslen(key);

    for (size_t i = 0; i < len; i++) {
        h = 31 * h + key[off++];
    }

    return h;
}

void TableDestroy(void *ptr) {
    Table *table = (Table *) ptr;

    for (int i = 0; i < table->cap; i++) {
        TablePair *pair = table->pairs[i];
        while (pair != NULL) {
            _delete(pair->key);
            _delete(pair->data);

            TablePair *temp = pair;
            pair = pair->next;

            free(temp);
        }
    }

    table->cap = 0;
    table->len = 0;
    free(table->pairs);
}

Table *TableCreate() {
    Table *table = create (Table, TableDestroy);

    table->cap = 32;
    table->len = 0;

    size_t size = sizeof(TablePair) * table->cap;
    table->pairs = malloc(size);
    memset(table->pairs, 0, size);

    return table;
}

TablePair *TableGetData(Table *table, const u16char *key) {
    if (table->cap <= 0) {
        Error(ERR_INVALID_INDEX);
        return NULL;
    }

    size_t addr = hash_string(key) % table->cap;
    TablePair *pair = table->pairs[addr];
    while (pair != NULL) {
        if (wcscmp(pair->key, key) == 0) {
            break;
        }

        pair = pair->next;
    }
    return pair;
}

TablePair *TableSetData(Table *table, const u16char *key, void *data) {
    if (table->cap <= 0) {
        Error(ERR_INVALID_INDEX);
        return NULL;
    }

    size_t addr = hash_string(key) % table->cap;
    TablePair *pair = table->pairs[addr];

    if (pair == NULL) {
        pair = malloc(sizeof(pair));

        pair->key = malloc(sizeof(u16char) * (wcslen(key) + 1));
        wcscpy(pair->key, key);
        pair->data = data;
        pair->next = NULL;

        table->pairs[addr] = pair;
    } else {
        while (pair != NULL) {
            if (wcscmp(pair->key, key) == 0) {
                pair->data = data;
                break;
            }

            if (pair->next != NULL) {
                pair = pair->next;
            } else {
                TablePair *new_pair = malloc(sizeof(pair));

                new_pair->key = malloc(sizeof(u16char) * (wcslen(key) + 1));
                wcscpy(new_pair->key, key);
                new_pair->data = data;
                new_pair->next = NULL;

                pair->next = new_pair;
                pair = new_pair;

                break;
            }
        }
    }

    table->len++;
    return pair;
}