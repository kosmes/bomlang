//
// Created by jiney on 2019-03-17.
//

#ifndef BINARY_TOKEN_H
#define BINARY_TOKEN_H

#include "common.h"

typedef enum token_type token_type_t;
typedef struct token token_t;

enum token_type {
    TokenNone,
    TokenComma,
    TokenAssign,
    TokenColom,
    TokenEqual, TokenNotEqual,
    TokenPlus, TokenMinus,
    TokenAbsterisk /* '*' */, TokenSlash /* / */, TokenModulus /* % */,
    TokenOpenBracket /* [ */, TokenCloseBracket /* ] */,
    TokenIdentifier, TokenIntegerConstant, TokenEPConstant /* floating point */, TokenStringConstant,
    TokenBoolConstant, TokenCharConstant,
    TokenSemicolon /* ; */,
    TokenLeftBrace /* { */, TokenRightBrace /* } */,
    TokenBoolType, TokenCharType, TokenIntType, TokenDoubleType,
    TokenEndOfFile, TokenEndOfLine, TokenEndOfFunction,
};

/**
 * @brief   토큰종류와 속성값으로 구성되는 데이터, 각 패턴에 부합하는 어휘 항목을 가지고 있는다
 */
struct token {
    /*  @brief  토큰의 종류 */
    token_type_t type;

    /*  @brief  토큰이 위치하는 줄 */
    unsigned int line;

    /*  @brief  토큰의 값 */
    union {
        char i8;
        int i32;
        double f64;
    };

    wchar_t *str;
};

/**
 * 토큰의 종류와 라인 값으로 토큰을 만든다
 * @param type  토큰의 종류
 * @param line  토큰의 라인
 * @return  초기화된 토큰
 */
DLL_EXPORT token_t create_token(token_type_t type, unsigned int line);

/**
 * 토큰을 파괴한다
 * @param token[in] 파괴할 토큰
 */
DLL_EXPORT void destroy_token(token_t *token);

#endif //BINARY_TOKEN_H
