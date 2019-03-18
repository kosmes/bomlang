//
// Created by jiney on 2019-03-17.
//

#include "node.h"
#include "buf.h"

DLL_EXPORT node_t *create_node(node_type_t type, token_t token, node_t **child) {
    node_t *node = malloc(sizeof(node_t));

    node->type = type;
    node->token = token;
    node->child = child;

    return node;
}

DLL_EXPORT void destroy_node(node_t *node) {
    for (int i = 0; i < buf_len(node->child); i++) {
        destroy_node(node->child[i]);
    }

    destroy_token(&node->token);
}