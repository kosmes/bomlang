//
// Created by jiney on 2019-03-17.
//

#include "compiler.h"
#include "buf.h"
#include "error.h"

#define COMBINE_BUFFER(src, dest) for (size_t i = 0; i < buf_len(dest); i++) { buf_push(src, dest[i]); }; \
    buf_free(dest); dest = NULL;

#define CODE_BUFFER unsigned char *
#define VISIT_PREPARE unsigned char *codes = NULL;
#define VISIT_RETURN return codes;

static Compiler *this;

static unsigned char *visit(node_t *node);

static TYPE_ID checkCasting(Script *target_script, TYPE_ID origin_type,
                            TYPE_ID to_type, bool force);

void CompilerInit(Compiler *compiler) {
    compiler->root_script = malloc(sizeof(Script));
    ScriptInit(compiler->root_script);

    compiler->scope_table = malloc(sizeof(Table));
    TableInit(compiler->scope_table);

    compiler->offset = 0;
}

void CompilerFinal(Compiler *compiler) {
    TableFinal(compiler->scope_table);
    ScriptFinal(compiler->root_script);
}

static CODE_BUFFER visitIntegerConstant(node_t *node) {
    VISIT_PREPARE;

    buf_push(codes, OP_CONST);

    Converter cvt;
    cvt.as_size = ScriptAddIntAndReturnAddr(this->root_script, node->token.i32);

    for (int i = 0; i < 8; i++) {
        buf_push(codes, cvt.as_bytes[i]);
    }

    VISIT_RETURN;
}

static CODE_BUFFER visitFPConstant(node_t *node) {
    VISIT_PREPARE;

    buf_push(codes, OP_CONST);

    Converter cvt;
    cvt.as_size = ScriptAddDoubleAndReturnAddr(this->root_script, node->token.f64);

    for (int i = 0; i < 8; i++) {
        buf_push(codes, cvt.as_bytes[i]);
    }

    VISIT_RETURN;
}

static CODE_BUFFER visitBinOp(node_t *node) {
    VISIT_PREPARE;

    node_t *lhs = node->child[0];
    node_t *rhs = node->child[1];

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

static CODE_BUFFER visitUnaryOp(node_t *node) {
    VISIT_PREPARE;

    CODE_BUFFER visit_codes = visit(node->child[0]);

    COMBINE_BUFFER(codes, visit_codes);

    if (node->token.type == TokenMinus) {
        buf_push(codes, OP_INVERT);
    }

    VISIT_RETURN;
}

static CODE_BUFFER visitAssignOp(node_t *node) {
    VISIT_PREPARE;

    node_t *lhs = node->child[0];
    node_t *rhs = node->child[1];

    if (lhs == NULL || rhs == NULL) {
        ErrorLine(ERR_NO_EXPR, node->token.line);
        return NULL;
    }

    TablePair *pair = TableGetData(this->scope_table, lhs->token.str);

    if (pair == NULL) {
        Error(ERR_UNDEF_VAR);
        return NULL;
    }

    size_t slot = *((size_t *) pair->data);

    CODE_BUFFER rhs_codes = visit(rhs);

    COMBINE_BUFFER(codes, rhs_codes);

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

static CODE_BUFFER visitVar(node_t *node) {
    VISIT_PREPARE;

    TablePair *pair = TableGetData(this->scope_table,
                                   node->token.str);

    if (pair == NULL) {
        Error(ERR_UNDEF_VAR);
        return NULL;
    }

    buf_push(codes, OP_LOAD);
    Converter cvt;
    cvt.as_size = *((size_t *) pair->data);
    for (int i = 0; i < 8; i++) {
        buf_push(codes, cvt.as_bytes[i]);
    }

    VISIT_RETURN;
}

static CODE_BUFFER visitVarDecl(node_t *node) {
    VISIT_PREPARE;

    node_t *var_node = node->child[0];
    node_t *type_node = node->child[1];
    node_t *init_node = node->child[2];

    if (var_node->type != NodeVar) {
        // #todo: Update error message to invalid decl
        Error(ERR_SYNTAX);
        return NULL;
    }

    TablePair *pair = TableGetData(this->scope_table, var_node->token.str);

    if (pair != NULL) {
        Error(ERR_DUPLE_VAR);
        return NULL;
    } else {
        size_t *slot_ptr = malloc(sizeof(size_t));
        *(slot_ptr) = this->offset++;
        if ((pair = TableSetData(this->scope_table,
                                 var_node->token.str, slot_ptr)) == NULL) {
            Error(ERR_ERROR);
            return NULL;
        }
    }

    size_t slot = *((size_t *) pair->data);

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

static CODE_BUFFER visitCompound(node_t *node) {
    VISIT_PREPARE;

    size_t len = buf_len(node->child);
    for (int i = 0; i < len; i++) {
        CODE_BUFFER visit_codes = visit(node->child[i]);
        COMBINE_BUFFER(codes, visit_codes);
    }

    VISIT_RETURN;
}

static CODE_BUFFER visit(node_t *node) {
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

bool CompilerTryCompile(Compiler *compiler, node_t *root_node) {
    this = compiler;


    CODE_BUFFER result = visit(root_node);

    if (ErrorGetCount() != 0) {
        buf_free(result);
        wprintf(L"오류 '%d'개가 검출되었습니다.\n", ErrorGetCount());
        return false;
    }

    COMBINE_BUFFER(this->root_script->text, result);

    buf_push(this->root_script->text, OP_DBG_PRNIT);
    buf_push(this->root_script->text, OP_RETURN);

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