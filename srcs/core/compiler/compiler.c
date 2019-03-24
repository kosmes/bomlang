//
// Created by jiney on 2019-03-17.
//

#include "compiler.h"
#include "container/buf.h"
#include "error.h"

#define COMBINE_BUFFER(src, dest) for (size_t i = 0; i < buf_len(dest); i++) { buf_push(src, dest[i]); }; \
    buf_free(dest); dest = NULL;

#define CODE_BUFFER unsigned char *
#define VISIT_PREPARE unsigned char *codes = NULL;
#define VISIT_RETURN return codes;

static Compiler *this;

static unsigned char *visit(Node *node);

static TYPE_ID checkCasting(Script *target_script, TYPE_ID origin_type,
                            TYPE_ID to_type, bool force);

void CompilerDestroy(void *ptr) {
    Compiler *compiler = (Compiler *) ptr;

    _delete(compiler->scopeTable);
    _delete(compiler->rootScript);
}

Compiler *CompilerCreate() {
    Compiler *compiler = create (Compiler, CompilerDestroy);

    compiler->rootScript = ScriptCreate();
    compiler->scopeTable = SymbolTableCreate(NULL);
    compiler->offset = 0;

    return compiler;
}

static CODE_BUFFER visitIntegerConstant(Node *node) {
    VISIT_PREPARE;

    buf_push(codes, OP_CONST);

    Converter cvt;
    cvt.as_size = ScriptAddIntAndReturnAddr(this->rootScript, node->token.i32);

    for (int i = 0; i < 8; i++) {
        buf_push(codes, cvt.as_bytes[i]);
    }

    VISIT_RETURN;
}

static CODE_BUFFER visitFPConstant(Node *node) {
    VISIT_PREPARE;

    buf_push(codes, OP_CONST);

    Converter cvt;
    cvt.as_size = ScriptAddDoubleAndReturnAddr(this->rootScript, node->token.f64);

    for (int i = 0; i < 8; i++) {
        buf_push(codes, cvt.as_bytes[i]);
    }

    VISIT_RETURN;
}

static CODE_BUFFER visitBinOp(Node *node) {
    VISIT_PREPARE;

    Node *lhs = node->child[0];
    Node *rhs = node->child[1];

    if (lhs == NULL || rhs == NULL) {
        ErrorLine(ERR_NO_EXPR, node->token.line);
    }

    unsigned char *lhs_codes = visit(lhs);
    unsigned char *rhs_codes = visit(rhs);

    COMBINE_BUFFER(codes, lhs_codes);
    COMBINE_BUFFER(codes, rhs_codes);

    switch (node->token.type) {
        case TokenPlus:
            buf_push(codes, OP_ADD);
            break;

        case TokenMinus:
            buf_push(codes, OP_SUB);
            break;

        case TokenAbsterisk:
            buf_push(codes, OP_MUL);
            break;

        case TokenSlash:
            buf_push(codes, OP_DIV);
            break;
    }

    VISIT_RETURN;
}

static CODE_BUFFER visitUnaryOp(Node *node) {
    VISIT_PREPARE;

    CODE_BUFFER visit_codes = visit(node->child[0]);

    COMBINE_BUFFER(codes, visit_codes);

    if (node->token.type == TokenMinus) {
        buf_push(codes, OP_INVERT);
    }

    VISIT_RETURN;
}

static CODE_BUFFER visitAssignOp(Node *node) {
    VISIT_PREPARE;

    Node *lhs = node->child[0];
    Node *rhs = node->child[1];

    if (lhs == NULL || rhs == NULL) {
        ErrorLine(ERR_NO_EXPR, node->token.line);
        return NULL;
    }

    Symbol *symbol = SymbolTableLookUp(this->scopeTable, lhs->token.str);

    if (symbol == NULL) {
        Error(ERR_UNDEF_VAR);
        return NULL;
    }

    CODE_BUFFER rhs_codes = visit(rhs);

    COMBINE_BUFFER(codes, rhs_codes);

    buf_push(codes, OP_STORE);

    Converter cvt;
    cvt.as_size = symbol->slot;

    for (int i = 0; i < 8; i++) {
        buf_push(codes, cvt.as_bytes[i]);
    }

    buf_push(codes, OP_LOAD);

    for (int i = 0; i < 8; i++) {
        buf_push(codes, cvt.as_bytes[i]);
    }

    VISIT_RETURN;
}

static CODE_BUFFER visitVar(Node *node) {
    VISIT_PREPARE;

    Symbol *symbol = SymbolTableLookUp(this->scopeTable, node->token.str);

    if (symbol == NULL) {
        Error(ERR_UNDEF_VAR);
        return NULL;
    }

    buf_push(codes, OP_LOAD);
    Converter cvt;
    cvt.as_size = symbol->slot;
    for (int i = 0; i < 8; i++) {
        buf_push(codes, cvt.as_bytes[i]);
    }

    VISIT_RETURN;
}

static CODE_BUFFER visitVarDecl(Node *node) {
    VISIT_PREPARE;

    Node *var_node = node->child[0];
    Node *type_node = node->child[1];
    Node *init_node = node->child[2];

    if (var_node->type != NodeVar) {
        // #todo: Update error message to invalid decl
        Error(ERR_SYNTAX);
        return NULL;
    }

    Symbol *symbol = SymbolTableLookUp(this->scopeTable, var_node->token.str);

    if (symbol != NULL) {
        Error(ERR_DUPLE_VAR);
        return NULL;
    } else {
        symbol = SymbolCreate(SYMBOL_TYPE_VAR, var_node->token.str, NULL);
        SymbolTableDefine(this->scopeTable, symbol);
        symbol->slot = this->offset++;
    }

    size_t slot = symbol->slot;

    CODE_BUFFER init_codes = visit(init_node);

    COMBINE_BUFFER(codes, init_codes);

    buf_push(codes, OP_STORE);

    Converter cvt;
    cvt.as_size = slot;

    for (int i = 0; i < 8; i++) {
        buf_push(codes, cvt.as_bytes[i]);
    }

    buf_push(codes, OP_LOAD);

    for (int i = 0; i < 8; i++) {
        buf_push(codes, cvt.as_bytes[i]);
    }

    VISIT_RETURN;
}

static CODE_BUFFER visitCompound(Node *node) {
    VISIT_PREPARE;

    size_t len = buf_len(node->child);
    for (int i = 0; i < len; i++) {
        CODE_BUFFER visit_codes = visit(node->child[i]);
        COMBINE_BUFFER(codes, visit_codes);
    }

    VISIT_RETURN;
}

static CODE_BUFFER visit(Node *node) {
    switch (node->type) {
        case NodeIntegerConstant:
            return visitIntegerConstant(node);

        case NodeFPConstant:
            return visitFPConstant(node);

        case NodeBinOp:
            return visitBinOp(node);

        case NodeUnaryOp:
            return visitUnaryOp(node);

        case NodeAssignOp:
            return visitAssignOp(node);

        case NodeVarDecl:
            return visitVarDecl(node);

        case NodeVar:
            return visitVar(node);

        case NodeCompound:
            return visitCompound(node);

        default: {
            return NULL;
        }
    }
}

bool CompilerTryCompile(Compiler *compiler, Node *root_node) {
    this = compiler;


    CODE_BUFFER result = visit(root_node);

    if (ErrorGetCount() != 0) {
        buf_free(result);
        wprintf(L"오류 '%d'개가 검출되었습니다.\n", ErrorGetCount());
        return false;
    }

    COMBINE_BUFFER(this->rootScript->text, result);

    buf_push(this->rootScript->text, OP_DBG_PRNIT);
    buf_push(this->rootScript->text, OP_RETURN);

    return true;
}

static TYPE_ID checkCasting(Script *target_script, TYPE_ID origin_type,
                            TYPE_ID to_type, bool force) {
    if (origin_type == TYPE_NONE || to_type == TYPE_NONE) {
        return TYPE_NONE;
    }

    if ((origin_type < to_type) || force) {
        buf_push(target_script->text, OP_CAST);
        buf_push(target_script->text, origin_type);
        buf_push(target_script->text, to_type);
        return to_type;
    }

    return origin_type;
}