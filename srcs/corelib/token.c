//
// Created by jiney on 2019-03-17.
//

#include "token.h"

token_t create_token(token_type_t type, unsigned int line) {
    token_t token;

    token.type = type;
    token.line = line;
    token.str = NULL;

    return token;
}

void destroy_token(token_t *token) {
    if (token->str != NULL) {
        free(token->str);
    }
}