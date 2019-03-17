//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_NODE_H
#define BOM_NODE_H

#include "common.h"

#include "token.h"

typedef struct node node_t;
typedef enum node_type node_type_t;

enum node_type {
    NodeIntegerConstant,
    NodeFPConstant,

    NodeBinOp,
};

struct node {
    node_type_t type;
    token_t token;
    node_t **child;
};

DLL_EXPORT node_t *create_node(node_type_t type, token_t token, node_t **child);
DLL_EXPORT void destroy_node(node_t *node);

#endif //BOM_NODE_H
