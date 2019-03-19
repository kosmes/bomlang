//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_NODE_H
#define BOM_NODE_H

#include "common.h"

#include "token.h"

typedef enum NODE_TYPES NODE_TYPES;
typedef struct node node_t;

enum NODE_TYPES {
    NodeIntegerConstant,
    NodeFPConstant,

    NodeBinOp,
    NodeUnaryOp,
    NodeAssignOp,

    NodeVar,

    // stmt
    NodeCompound,
    NodeEmpty,
};

struct node {
    NODE_TYPES type;
    token_t token;
    node_t **child;
};

DLL_EXPORT node_t *create_node(NODE_TYPES type, token_t token, node_t **child);
DLL_EXPORT void destroy_node(node_t *node);

#endif //BOM_NODE_H
