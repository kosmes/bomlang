//
// Created by jiney on 2019-03-17.
//

#include "parser.h"
#include "buf.h"
#include "errno.h"

struct parser {
    token_t *tokens;
    token_t token;
    size_t pos;
} this;

static node_t *level1();

static node_t *level3();

static node_t *level4();

static node_t *expr();

DLL_EXPORT node_t *do_parse(token_t *tokens) {
    if (tokens != NULL) {

        this.tokens = tokens;
        this.pos = 0;
        this.token = this.tokens[this.pos];

    } else {
        return NULL;
    }

    return expr();
}

static void eat(token_type_t type) {
    if (this.token.type != type) {
        error(SYNTAX_ERR, this.token.line);
        this.token.type = TokenNone;
    } else {
        this.pos += 1;
        if (this.pos > buf_len(this.tokens)) {
            error(SYNTAX_ERR, this.token.line);
        } else {
            this.token = this.tokens[this.pos];
        }
    }
}

static node_t *level1() {
    node_t **child = NULL;
    token_t tkn = this.token;
    if (this.token.type == TokenPlus) {
        eat(TokenPlus);
        buf_push(child, level1());
        return create_node(NodeUnaryOp, tkn, child);
    } else if (this.token.type == TokenMinus) {
        eat(TokenMinus);
        buf_push(child, level1());
        return create_node(NodeUnaryOp, tkn, child);
    } else if (this.token.type == TokenIntegerConstant) {
        eat(TokenIntegerConstant);
        return create_node(NodeIntegerConstant, tkn, NULL);
    } else if (this.token.type == TokenFPConstant) {
        eat(TokenFPConstant);
        return create_node(NodeFPConstant, tkn, NULL);
    } else if (this.token.type == TokenLeftParen) {
        eat(TokenLeftParen);
        node_t *node = expr();
        eat(TokenRightParen);
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
            eat(TokenAbsterisk);
        } else if (this.token.type == TokenSlash) {
            eat(TokenSlash);
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
            eat(TokenPlus);
        } else if (this.token.type == TokenMinus) {
            eat(TokenMinus);
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

