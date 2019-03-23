//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_COMPILER_H
#define BOM_COMPILER_H

#include "common.h"
#include "script.h"
#include "visitor.h"
#include "container/table.h"

typedef struct compiler Compiler;

struct compiler {
    Script *rootScript;
    Table *scopeTable;
    size_t offset;
};

DLL_EXPORT Compiler *CompilerCreate();

DLL_EXPORT bool CompilerTryCompile(Compiler *compiler, Node *root_node);

#endif //BOM_COMPILER_H
