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

DLL_EXPORT size_t add_int_and_return_addr(script_t *script, int data);

DLL_EXPORT size_t add_double_and_return_addr(script_t *script, double data);

DLL_EXPORT size_t add_string_and_return_addr(script_t *script, const wchar_t *data);

DLL_EXPORT int get_int_from_addr(script_t *script, size_t addr);

DLL_EXPORT double get_double_from_addr(script_t *script, size_t addr);

DLL_EXPORT const wchar_t *get_string_from_addr(script_t *script, size_t addr);

#endif //BOM_SCRIPT_H
