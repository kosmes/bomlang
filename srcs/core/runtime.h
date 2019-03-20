//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_OPCODE_H
#define BOM_OPCODE_H

typedef enum TYPE_IDS TYPE_IDS;
typedef enum OP_CODES OP_CODES;

/**
 * @brief   아무 정보 형이든 다 가질 수 있는 구조체
 */
typedef struct var var_t;

enum TYPE_IDS {
    TYPE_NONE,
    TYPE_AUTO,
    TYPE_INT,
    TYPE_DOUBLE,
};

enum OP_CODES {
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

    OP_INVERT,

    OP_DBG_PRNIT,

    OP_RETURN,
};

/**
 * @brief   아무 정보 형이든 다 가질 수 있는 구조체
 */
struct var {
    TYPE_IDS type_id;           /* < 변수 종류 */
    void *data;                 /* < 변수 값 */
    const u16char *dbg_name;    /* < 디버그용 변수 이름 */
};

#endif //BOM_OPCODE_H
