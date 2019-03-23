//
// Created by jiney on 2019-03-17.
//

#include "token.h"

Token TokenCreate(TOKEN_TYPE type, unsigned int line) {
    Token token;

    token.type = type;
    token.line = line;
    token.str = NULL;

    return token;
}

void TokenDestroy(Token *token) {
    if (token->str != NULL) {
        free(token->str);
    }
}