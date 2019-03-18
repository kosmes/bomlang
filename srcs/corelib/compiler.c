//
// Created by jiney on 2019-03-17.
//

#include "compiler.h"
#include "buf.h"
#include "error.h"

#define COMBINE_BUFFER(src, dest) for (size_t i = 0; i < buf_len(dest); i++) { buf_push(src, dest[i]); }

static script_t *root_script;

static visit_result_t visit(node_t *node);

static TYPE_CODES check_casting(script_t *target_script, TYPE_CODES origin_type, 
                                 TYPE_CODES to_type, bool force);


static visit_result_t visit_bin_op(node_t *node) {
    visit_result_t result;
    result.script = malloc(sizeof(script_t));

    init_script(result.script);

    node_t *lhs = node->child[0];
    node_t *rhs = node->child[1];

    if (lhs == NULL || rhs == NULL) {
        error(NO_EXPR, node->token.line);
    }

    visit_result_t lhs_result = visit(lhs);
    visit_result_t rhs_result = visit(rhs);

    result.type_id = check_casting(lhs_result.script, lhs_result.type_id, rhs_result.type_id, false);
    result.type_id = check_casting(rhs_result.script, rhs_result.type_id, lhs_result.type_id, false);

    size_t len = buf_len(lhs_result.script->text);
    for (size_t i = 0; i < len; i++) {
        buf_push(result.script->text, lhs_result.script->text[i]);
    }
    final_script(lhs_result.script);

    len = buf_len(rhs_result.script->text);
    for (size_t i = 0; i < len; i++) {
        buf_push(result.script->text, rhs_result.script->text[i]);
    }
    final_script(rhs_result.script);

    switch(node->token.type) {
        case TokenPlus:
            buf_push(result.script->text, OP_ADD);
            buf_push(result.script->text, result.type_id);
            break;

        case TokenMinus:
            buf_push(result.script->text, OP_SUB);
            buf_push(result.script->text, result.type_id);
            break;

        case TokenAbsterisk:
            buf_push(result.script->text, OP_MUL);
            buf_push(result.script->text, result.type_id);
            break;

        case TokenSlash:
            buf_push(result.script->text, OP_DIV);
            buf_push(result.script->text, result.type_id);
            break;
    }

    return result;
}

static visit_result_t visit_unary_op(node_t *node) {
    visit_result_t result;
    result.script = malloc(sizeof(script_t));
    init_script(result.script);

    visit_result_t visit_result = visit(node->child[0]);
    result.type_id = visit_result.type_id;

    COMBINE_BUFFER(result.script->text, visit_result.script->text);
    final_script(visit_result.script);

    if (node->token.type == TokenMinus) {
        buf_push(result.script->text, OP_INVERT);
        buf_push(result.script->text, result.type_id);
    }

    return result;
}

static visit_result_t visit_integer_constant(node_t *node) {
    visit_result_t result;
    result.script = malloc(sizeof(script_t));
    result.type_id = TYPE_INT;

    init_script(result.script);

    buf_push(result.script->text, OP_CONST);
    buf_push(result.script->text, TYPE_INT);

    converter_t cvt;
    cvt.asSize = add_int_and_return_addr(root_script, node->token.i32);

    for (int i = 0; i < 8; i++) {
        buf_push(result.script->text, cvt.asBytes[i]);
    }

    return result;
}

static visit_result_t visit_fp_constant(node_t *node) {
    visit_result_t result;
    result.script = malloc(sizeof(script_t));
    result.type_id = TYPE_DOUBLE;

    init_script(result.script);

    buf_push(result.script->text, OP_CONST);
    buf_push(result.script->text, TYPE_DOUBLE);

    converter_t cvt;
    cvt.asSize = add_double_and_return_addr(root_script, node->token.f64);

    for (int i = 0; i < 8; i++) {
        buf_push(result.script->text, cvt.asBytes[i]);
    }

    return result;
}

static visit_result_t visit(node_t *node) {
    switch (node->type) {
    case NodeIntegerConstant:
        return visit_integer_constant(node);

    case NodeFPConstant:
        return visit_fp_constant(node);

    case NodeBinOp:
        return visit_bin_op(node);

    case NodeUnaryOp:
        return visit_unary_op(node);

    default:{
        visit_result_t result;

        result.script = NULL;
        result.type_id = TYPE_NONE;

        return result;
    } }
}

script_t *compile(node_t *root_node) {
    root_script = malloc(sizeof(script_t));
    init_script(root_script);

    visit_result_t result = visit(root_node);

    size_t len = buf_len(result.script->text);
    for (size_t i = 0; i < len; i++) {
        buf_push(root_script->text, result.script->text[i]);
    }
    final_script(result.script);

    buf_push(root_script->text, OP_DBG_PRNIT);
    buf_push(root_script->text, result.type_id);

    buf_push(root_script->text, OP_HALT);

    return root_script;
}

static TYPE_CODES check_casting(script_t *target_script, TYPE_CODES origin_type, 
                                 TYPE_CODES to_type, bool force) {
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