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
typedef enum NODE_TYPE NODE_TYPE;

/**
 * @brief   트리형으로 구성된 AST 노드
 */
typedef struct node Node;

/**
 * @brief   노드 종류들을 모아놓은 열거형
 */
enum NODE_TYPE {
    NodeIntegerConstant,    /* < 정수 상수 */
    NodeFPConstant,         /* < 실수 상수 */

    NodeBinOp,              /* < 2항 연산식 */
    NodeUnaryOp,            /* < 단항 연산 */
    NodeAssignOp,           /* < 대입식 */
    NodeVarDecl,            /* < 변수 선언 식 */

    NodeVar,                /* < 변수 참조식 */

    // stmt
    NodeCompound,           /* < 복문 */
    NodeEmpty,              /* < 빈 식*/


    NodeType,
};

/**
 * @brief   트리형으로 구성된 AST 노드
 */
struct node {
    NODE_TYPE type;    /* < 노드의 종류 */
    Token token;      /* < 노드의 토큰 */
    Node **child;     /* < 노드의 자식들 */
};

/**
 * @brief   주어진 정보로 노드를 생성한다
 * @param   type    노드의 종류
 * @param   token   노드의 토큰
 * @param   child   노드의 자식들
 * @return  생성된 노드
 */
DLL_EXPORT Node *NodeCreate(NODE_TYPE type, Token token, Node **child);

DLL_EXPORT void NodePrint(Node *node, int indent);

#endif //BOM_NODE_H
