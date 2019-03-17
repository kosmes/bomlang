//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_OPCODE_H
#define BOM_OPCODE_H

typedef enum type_code type_code_t;
typedef enum op_code op_code_t;

enum type_code {
    TYPE_NONE,
    TYPE_INT,
    TYPE_DOUBLE,
};

enum op_code {
    OP_HALT,

    OP_CONST = 0x10 ,
    OP_STORE,
    OP_LOAD,

    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,

    /**
     * @brief   스택의 꼭대기서부터 2개를 비교한 후 결과를 스택에 집어넣는다.
     * @code    OP_CAST '비교할 타입'
     */
    OP_CMP,

    /**
     * @brief   스택의 꼭대기의 값을 다른 형으로 변환한다.
     * @code    OP_CAST '바꿀 타입'
     */
    OP_CAST,
};

#endif //BOM_OPCODE_H
