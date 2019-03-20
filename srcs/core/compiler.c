﻿//
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

static compiler_t *this;

static unsigned char *visit(node_t *node);

static TYPE_IDS check_casting(script_t *target_script, TYPE_IDS origin_type,
                              TYPE_IDS to_type, bool force);

void init_compiler(compiler_t *compiler) {
    compiler->root_script = malloc(sizeof(script_t));
    init_script(compiler->root_script);

    compiler->scope_table = malloc(sizeof(table_t));
    init_table(compiler->scope_table);

    compiler->offset = 0;
}

void final_compiler(compiler_t *compiler) {
    final_table(compiler->scope_table);
    final_script(compiler->root_script);
}

static CODE_BUFFER visit_integer_constant(node_t *node) {
    VISIT_PREPARE;

    buf_push(codes, OP_CONST);

    converter_t cvt;
    cvt.as_size = add_int_and_return_addr(this->root_script, node->token.i32);

    for (int i = 0; i < 8; i++) {
        buf_push(codes, cvt.as_bytes[i]);
    }

    VISIT_RETURN;
}

static CODE_BUFFER visit_fp_constant(node_t *node) {
    VISIT_PREPARE;

    buf_push(codes, OP_CONST);

    converter_t cvt;
    cvt.as_size = add_double_and_return_addr(this->root_script, node->token.f64);

    for (int i = 0; i < 8; i++) {
        buf_push(codes, cvt.as_bytes[i]);
    }

    VISIT_RETURN;
}

static CODE_BUFFER visit_bin_op(node_t *node) {
    VISIT_PREPARE;

    node_t *lhs = node->child[0];
    node_t *rhs = node->child[1];

    if (lhs == NULL || rhs == NULL) {
        error_line(ERR_NO_EXPR, node->token.line);
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

static CODE_BUFFER visit_unary_op(node_t *node) {
    VISIT_PREPARE;

    CODE_BUFFER visit_codes = visit(node->child[0]);

    COMBINE_BUFFER(codes, visit_codes);

    if (node->token.type == TokenMinus) {
        buf_push(codes, OP_INVERT);
    }

    VISIT_RETURN;
}

static CODE_BUFFER visit_assign_op(node_t *node) {
    VISIT_PREPARE;

    node_t *lhs = node->child[0];
    node_t *rhs = node->child[1];

    if (lhs == NULL || rhs == NULL) {
        error_line(ERR_NO_EXPR, node->token.line);
        return NULL;
    }

    table_pair_t *pair = table_find_by_key(this->scope_table, lhs->token.str);

    if (pair == NULL) {
        size_t *slot_ptr = malloc(sizeof(size_t));
        *(slot_ptr) = this->offset++;
        if ((pair = table_insert_data(this->scope_table,
                                      lhs->token.str, slot_ptr)) == NULL) {
            error(ERR_ERROR);
            return NULL;
        }
    }

    size_t slot = *((size_t *) pair->data);

    CODE_BUFFER rhs_codes = visit(rhs);

    COMBINE_BUFFER(codes, rhs_codes);

    buf_push(codes, OP_STORE);

    converter_t cvt;
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

static CODE_BUFFER visit_var(node_t *node) {
    VISIT_PREPARE;

    table_pair_t *pair = table_find_by_key(this->scope_table,
                                           node->token.str);

    if (pair == NULL) {
        error(ERR_UNDEF_VAR);
        return NULL;
    }

    buf_push(codes, OP_LOAD);
    converter_t cvt;
    cvt.as_size = *((size_t *) pair->data);
    for (int i = 0; i < 8; i++) {
        buf_push(codes, cvt.as_bytes[i]);
    }

    VISIT_RETURN;
}

static CODE_BUFFER visit_compound(node_t *node) {
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
            return visit_integer_constant(node);

        case NodeFPConstant:
            return visit_fp_constant(node);

        case NodeBinOp:
            return visit_bin_op(node);

        case NodeUnaryOp:
            return visit_unary_op(node);

        case NodeAssignOp:
            return visit_assign_op(node);

        case NodeVar:
            return visit_var(node);

        case NodeCompound:
            return visit_compound(node);

        default: {
            return NULL;
        }
    }
}

bool compile(compiler_t *compiler, node_t *root_node) {
    this = compiler;


    CODE_BUFFER result = visit(root_node);

    if (get_error_count() != 0) {
        buf_free(result);
        wprintf(L"오류 '%d'개가 검출되었습니다.\n", get_error_count());
        return false;
    }

    COMBINE_BUFFER(this->root_script->text, result);

    buf_push(this->root_script->text, OP_DBG_PRNIT);
    buf_push(this->root_script->text, OP_RETURN);

    return true;
}

static TYPE_IDS check_casting(script_t *target_script, TYPE_IDS origin_type,
                                TYPE_IDS to_type, bool force) {
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