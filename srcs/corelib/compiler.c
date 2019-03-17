//
// Created by jiney on 2019-03-17.
//

#include "compiler.h"
#include "buf.h"

script_t *visit_integer_constant(node_t *node) {
    script_t *script = malloc(sizeof(script_t));

    return script;
}

script_t *visit(node_t *node) {
    switch(node->type) {
        case NodeIntegerConstant:
            return visit_integer_constant(node);
        default:
            return NULL;
    }
}

script_t *compile(node_t *root_node) {
    script_t *script = NULL;

    script = visit(root_node);

    return script;
}