//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_NODE_H
#define BOM_NODE_H

#include "common.h"

#include "token.h"

/**
 * @brief   노드 종류들을 모아놓은 열거형
 */
typedef enum NODE_TYPES NODE_TYPES;

/**
 * @brief   트리형으로 구성된 AST 노드
 */
typedef struct node node_t;

/**
 * @brief   노드 종류들을 모아놓은 열거형
 */
enum NODE_TYPES {
    NodeIntegerConstant,    /* < 정수 상수 */
    NodeFPConstant,         /* < 실수 상수 */

    NodeBinOp,              /* < 2항 연산식 */
    NodeUnaryOp,            /* < 단항 연산 */
    NodeAssignOp,           /* < 대입식 */

    NodeVar,                /* < 변수 참조식 */

    // stmt
    NodeCompound,           /* < 복문 */
    NodeEmpty,              /* < 빈 식*/
};

/**
 * @brief   트리형으로 구성된 AST 노드
 */
struct node {
    NODE_TYPES type;    /* < 노드의 종류 */
    token_t token;      /* < 노드의 토큰 */
    node_t **child;     /* < 노드의 자식들 */
};

/**
 * @brief   주어진 정보로 노드를 생성한다
 * @param   type    노드의 종류
 * @param   token   노드의 토큰
 * @param   child   노드의 자식들
 * @return  생성된 노드
 */
DLL_EXPORT node_t *create_node(NODE_TYPES type, token_t token, node_t **child);

/**
 * @brief   노드의 자식들을 삭제하고 파괴한다
 * @param   node    파괴할 노드
 */
DLL_EXPORT void destroy_node(node_t *node);

#endif //BOM_NODE_H
