//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_COMPILER_H
#define BOM_COMPILER_H

#include "common.h"
#include "script.h"
#include "node.h"

DLL_EXPORT script_t *compile(node_t *root_node);

#endif //BOM_COMPILER_H
