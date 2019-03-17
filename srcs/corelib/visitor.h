//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_BUILDER_H
#define BOM_BUILDER_H

#include "node.h"
#include "runtime.h"

typedef struct visit_result visit_result_t;

struct visit_result {
    script_t *script;
    type_code_t type_id;
};

#endif //BOM_BUILDER_H