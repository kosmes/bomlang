//
// Created by jiney on 2019-03-17.
//

#include "node.h"
#include "container/buf.h"

void NodeDestroy(void *ptr) {
    Node *node = (Node *) ptr;
    for (int i = 0; i < buf_len(node->child); i++) {
        NodeDestroy(node->child[i]);
    }

    TokenDestroy(&node->token);
}

DLL_EXPORT Node *NodeCreate(NODE_TYPE type, Token token, Node **child) {
    Node *node = create (Node, NodeDestroy);

    node->type = type;
    node->token = token;
    node->child = child;

    return node;
}

void NodePrint(Node *node, int indent) {
    if (node == NULL) {
        return;
    }

    for (int i = 0; i < indent; i++) {
        wprintf(L"\t");
    }

    wprintf(L"노드: ");
    switch(node->type) {
        case NodeIntegerConstant:
            wprintf(L"NodeIntegerConstant");
            wprintf(L" 정수 값: %d\n", node->token.i32);
            break;

        case NodeFPConstant:
            wprintf(L"NodeFPConstant");
            wprintf(L" 실수 값: %lf\n", node->token.f64);
            break;

        case NodeAssignOp:
            wprintf(L"NodeAssignOp\n");
            break;

        case NodeCompound:
            wprintf(L"NodeCompound\n");
            break;

        case NodeUnaryOp:
            wprintf(L"NodeUnaryOp\n");
            break;

        case NodeVar:
            wprintf(L"NodeVar");
            wprintf(L" 변수 값 : %ls\n", node->token.str);
            break;

        case NodeVarDecl:
            wprintf(L"NodeVarDecl\n");
            break;

        case NodeEmpty:
            wprintf(L"NodeEmpty\n");
            break;

        case NodeType:
            wprintf(L"NodeType");
            switch (node->token.type) {
                case TokenAutoType:
                    wprintf(L" \t형: 자동 \n");
                    break;

                case TokenIntType:
                    wprintf(L" \t형: 정수 \n");
                    break;

                case TokenDoubleType:
                    wprintf(L" \t형: 실수 \n");
                    break;

                default:
                    wprintf(L"\n");
                    break;
            }
            break;

        default:
            wprintf(L"\n");
            break;
    }

    for(int i = 0; i < buf_len(node->child); i++) {
        NodePrint(node->child[i], indent + 1);
    }
}
