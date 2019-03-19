﻿//
// Created by jiney on 2019-03-17.
//

#include "parser.h"
#include "buf.h"
#include "error.h"

typedef struct parser parser_t;

struct parser {
    token_t *tokens;
    token_t token;
    size_t pos;

    unsigned short err_count;
};

static parser_t this;

static node_t *level1();

static node_t *level3();

static node_t *level4();

static node_t *expr();

DLL_EXPORT node_t *do_parse(token_t *tokens) {
    if (tokens != NULL) {

        this.err_count = 0;

        this.tokens = tokens;
        this.pos = 0;
        this.token = this.tokens[this.pos];

    } else {
        return NULL;
    }

    node_t *root_node = expr();
    if (this.token.type != TokenEndOfFile) {
        error(ERR_SYNTAX, this.token.line);
        this.err_count++;
    }
    if (this.err_count != 0) {
        destroy_node(root_node);
        wprintf(L"오류 '%d'개가 검출되었습니다.\n", this.err_count);
        return NULL;
    } else {
        return root_node;
    }
}

static void eat(TOKEN_TYPES type, enum ERROR_CODE errcode) {
    if (this.token.type != type) {
        error(errcode, this.token.line);

        this.token.type = TokenNone;
        this.err_count++;
    } else {
        this.pos += 1;
        if (this.pos > buf_len(this.tokens)) {
            error(ERR_SYNTAX, this.token.line);

            this.err_count++;
        } else {
            this.token = this.tokens[this.pos];
        }
    }
}

static node_t *level1() {
    node_t **child = NULL;
    token_t tkn = this.token;
    if (this.token.type == TokenPlus) {
        eat(TokenPlus, ERR_SYNTAX);
        buf_push(child, level1());
        return create_node(NodeUnaryOp, tkn, child);
    } else if (this.token.type == TokenMinus) {
        eat(TokenMinus, ERR_SYNTAX);
        buf_push(child, level1());
        return create_node(NodeUnaryOp, tkn, child);
    } else if (this.token.type == TokenIntegerConstant) {
        eat(TokenIntegerConstant, ERR_SYNTAX);
        return create_node(NodeIntegerConstant, tkn, NULL);
    } else if (this.token.type == TokenFPConstant) {
        eat(TokenFPConstant, ERR_SYNTAX);
        return create_node(NodeFPConstant, tkn, NULL);
    } else if (this.token.type == TokenLeftParen) {
        eat(TokenLeftParen, ERR_SYNTAX);
        node_t *node = expr();
        eat(TokenRightParen, ERR_UNBAL_PARENS);
        return node;
    }

    return NULL;
}

static node_t *level3() {
    node_t *node = level1();

    while (this.token.type == TokenAbsterisk ||
           this.token.type == TokenSlash) {
        token_t tkn = this.token;
        if (this.token.type == TokenAbsterisk) {
            eat(TokenAbsterisk, ERR_SYNTAX);
        } else if (this.token.type == TokenSlash) {
            eat(TokenSlash, ERR_SYNTAX);
        }

        node_t **child = NULL;
        buf_push(child, node);
        buf_push(child, level1());

        node = create_node(NodeBinOp, tkn, child);
    }

    return node;
}

static node_t *level4() {
    node_t *node = level3();

    while (this.token.type == TokenPlus ||
           this.token.type == TokenMinus) {
        token_t tkn = this.token;
        if (this.token.type == TokenPlus) {
            eat(TokenPlus, ERR_SYNTAX);
        } else if (this.token.type == TokenMinus) {
            eat(TokenMinus, ERR_SYNTAX);
        }

        node_t **child = NULL;
        buf_push(child, node);
        buf_push(child, level3());

        node = create_node(NodeBinOp, tkn, child);
    }

    return node;
}

static node_t *expr() {
    return level4();
}
