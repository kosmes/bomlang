//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_SCRIPT_H
#define BOM_SCRIPT_H

#include "common.h"

typedef struct script script_t;

struct script {
    unsigned char *data;
    unsigned char *text;
};

DLL_EXPORT void init_script(script_t *script);

DLL_EXPORT void final_script(script_t *script);

DLL_EXPORT void add_int_and_put_addr(script_t *script, int data);

DLL_EXPORT void add_double_and_put_addr(script_t *script, double data);

DLL_EXPORT void add_string_and_put_addr(script_t *script, const wchar_t *data);

#endif //BOM_SCRIPT_H
