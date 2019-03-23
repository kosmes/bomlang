//
// Created by jiney on 2019-03-17.
//

#include "parser.h"
#include "container/buf.h"
#include "error.h"

typedef struct parser Parser;

struct parser {
    Token *tokens;
    Token token;
    size_t pos;
};

static Parser this;

static Node *level1();

static Node *level3();

static Node *level4();

static Node *expr();

static Node *compound();

static Node **statementList();

static Node *statement();

static Node *var_declare();

static Node *assign();

static Node *variable();


DLL_EXPORT Node *ParserDoParse(Token *tokens) {
    if (tokens != NULL) {

        this.tokens = tokens;
        this.pos = 0;
        this.token = this.tokens[this.pos];

    } else {
        return NULL;
    }

    Node *root_node = compound();
    if (this.token.type != TokenEndOfFile) {
        ErrorLine(ERR_SYNTAX, this.token.line);
    }
    if (ErrorGetCount() != 0) {
        delete(root_node);
        wprintf(L"오류 '%d'개가 검출되었습니다.\n", ErrorGetCount());
        return NULL;
    } else {
        return root_node;
    }
}

static void eat(TOKEN_TYPE type, enum ERROR_CODE errcode) {
    if (this.token.type != type) {
        ErrorLine(errcode, this.token.line);

        this.token.type = TokenNone;
    } else {
        this.pos += 1;
        if (this.pos > buf_len(this.tokens)) {
            ErrorLine(ERR_INVALID_EOF, this.token.line);
        } else {
            this.token = this.tokens[this.pos];
        }
    }
}

static Node *level1() {
    Node **child = NULL;
    Token tkn = this.token;
    if (this.token.type == TokenPlus) {
        eat(TokenPlus, ERR_SYNTAX);
        buf_push(child, level1());
        return NodeCreate(NodeUnaryOp, tkn, child);
    } else if (this.token.type == TokenMinus) {
        eat(TokenMinus, ERR_SYNTAX);
        buf_push(child, level1());
        return NodeCreate(NodeUnaryOp, tkn, child);
    } else if (this.token.type == TokenIntegerConstant) {
        eat(TokenIntegerConstant, ERR_SYNTAX);
        return NodeCreate(NodeIntegerConstant, tkn, NULL);
    } else if (this.token.type == TokenFPConstant) {
        eat(TokenFPConstant, ERR_SYNTAX);
        return NodeCreate(NodeFPConstant, tkn, NULL);
    } else if (this.token.type == TokenLeftParen) {
        eat(TokenLeftParen, ERR_SYNTAX);
        Node *node = expr();
        eat(TokenRightParen, ERR_UNBAL_PARENS);
        return node;
    } else {
        return variable();
    }

    return NULL;
}

static Node *level3() {
    Node *node = level1();

    while (this.token.type == TokenAbsterisk ||
           this.token.type == TokenSlash) {
        Token tkn = this.token;
        if (this.token.type == TokenAbsterisk) {
            eat(TokenAbsterisk, ERR_SYNTAX);
        } else if (this.token.type == TokenSlash) {
            eat(TokenSlash, ERR_SYNTAX);
        }

        Node **child = NULL;
        buf_push(child, node);
        buf_push(child, level1());

        node = NodeCreate(NodeBinOp, tkn, child);
    }

    return node;
}

static Node *level4() {
    Node *node = level3();

    while (this.token.type == TokenPlus ||
           this.token.type == TokenMinus) {
        Token tkn = this.token;
        if (this.token.type == TokenPlus) {
            eat(TokenPlus, ERR_SYNTAX);
        } else if (this.token.type == TokenMinus) {
            eat(TokenMinus, ERR_SYNTAX);
        }

        Node **child = NULL;
        buf_push(child, node);
        buf_push(child, level3());

        node = NodeCreate(NodeBinOp, tkn, child);
    }

    return node;
}

static Node *expr() {
    return level4();
}

static Node *compound() {
    return NodeCreate(NodeCompound, TokenCreate(TokenNone, 0), statementList());
}

static Node **statementList() {
    Node **result = NULL;
    buf_push(result, statement());

    while (this.token.type == TokenSemicolon) {
        eat(TokenSemicolon, ERR_SEMI_EXPECTED);
        buf_push(result, statement());
    }

    if (this.token.type == TokenIdentifier) {
        ErrorLine(ERR_SEMI_EXPECTED, this.token.line);
    }

    return result;
}

static Node *statement() {
    size_t offset = this.pos;
    Token last_token;
    while (buf_len(this.tokens) > offset) {
        offset++;
        if (this.tokens[offset].type == TokenSemicolon) {
            break;
        }

        if (this.tokens[offset].type == TokenEndOfFile) {
            ErrorLine(ERR_INVALID_EOF, this.token.line);
        }

        last_token = this.tokens[offset];
    }

    if (this.token.type == TokenIdentifier) {
        if (last_token.type == TokenVarDecl) {
            return var_declare();
        } else {
            return assign();
        }
    } else {
        return NodeCreate(NodeEmpty, this.token, NULL);
    }
}

static Node *var_declare() {
    Node **declare = NULL;

    do {
        if (this.token.type == TokenComma) {
            eat(TokenComma, ERR_SEMI_EXPECTED);
        }

        Node **child = NULL;
        buf_push(child, variable());

        if (this.token.type == TokenColon) {
            eat(TokenColon, ERR_SEMI_EXPECTED);
            Token tkn = this.token;
            if (this.token.type == TokenIntType) {
                eat(TokenIntType, ERR_SEMI_EXPECTED);
            } else if (this.token.type == TokenDoubleType) {
                eat(TokenDoubleType, ERR_SEMI_EXPECTED);
            } else {
                eat(TokenNone, ERR_TYPE_EXPECTED);
            }
            buf_push(child, NodeCreate(NodeType, tkn, NULL));
        } else {
            Token tkn = {.type=TokenAutoType, .i32=0, .str=NULL, .line=this.token.line};
            buf_push(child, NodeCreate(NodeType, tkn, NULL));
        }

        if (this.token.type == TokenAssign) {
            eat(TokenAssign, ERR_SEMI_EXPECTED);
            buf_push(child, expr());
        } else {
            Token tkn = {.type=TokenIntegerConstant, .i32=0, .str=NULL, .line=this.token.line};
            buf_push(child, NodeCreate(NodeIntegerConstant, tkn, NULL));
        }

        buf_push(declare, NodeCreate(NodeVarDecl, this.token, child));

    } while (this.token.type == TokenComma);

    eat(TokenVarDecl, ERR_SEMI_EXPECTED);

    return NodeCreate(NodeCompound, this.token, declare);
}

static Node *assign() {
    Node **child = NULL;
    buf_push(child, variable());
    Token tkn = this.token;
    eat(TokenAssign, ERR_SYNTAX);
    buf_push(child, expr());
    return NodeCreate(NodeAssignOp, tkn, child);
}

static Node *variable() {
    Node *node = NodeCreate(NodeVar, this.token, NULL);
    eat(TokenIdentifier, ERR_ID_EXPECTED);
    return node;
}