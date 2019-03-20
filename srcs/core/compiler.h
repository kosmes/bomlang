//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_COMPILER_H
#define BOM_COMPILER_H

#include "common.h"
#include "script.h"
#include "visitor.h"
#include "table.h"

typedef struct compiler compiler_t;

struct compiler {
    script_t *root_script;
    table_t *scope_table;
    size_t offset;
};

DLL_EXPORT void init_compiler(compiler_t *compiler);

DLL_EXPORT void final_compiler(compiler_t *compiler);

DLL_EXPORT bool compile(compiler_t *compiler, node_t *root_node);

#endif //BOM_COMPILER_H
