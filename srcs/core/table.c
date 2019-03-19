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

void init_table(table_t *table) {
    table->cap = 32;
    table->len = 0;

    size_t size = sizeof(table_pair_t) * table->cap;
    table->pairs = malloc(size);
    memset(table->pairs, 0, size);
}

void final_table(table_t *table) {
    for (int i = 0; i < table->cap; i++) {
        table_pair_t *pair = table->pairs[i];
        while (pair != NULL) {
            free(pair->key);
            free(pair->data);

            table_pair_t *temp = pair;
            pair = pair->next;

            free(temp);
        }
    }

    table->cap = 0;
    table->len = 0;
    free(table->pairs);
}

table_pair_t *find_by_key(table_t *table, const u16char *key) {
    if (table->cap <= 0) {
        error(ERR_INVALID_INDEX);
        return NULL;
    }

    size_t addr = hash_string(key) % table->cap;
    return table->pairs[addr];
}

table_pair_t *insert_value(table_t *table, const u16char *key, void *data) {
    if (table->cap <= 0) {
        error(ERR_INVALID_INDEX);
        return false;
    }

    size_t addr = hash_string(key) % table->cap;
    table_pair_t *pair = table->pairs[addr];

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
                table_pair_t *new_pair = malloc(sizeof(pair));

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