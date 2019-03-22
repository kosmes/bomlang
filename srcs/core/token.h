//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_TOKEN_H
#define BOM_TOKEN_H

#include "common.h"

/**
 * @brief   토큰의 종류들을 모아놓은 열거형
 */
typedef enum TOKEN_TYPE TOKEN_TYPE;

/**
 * @brief   토큰종류와 속성값으로 구성되는 데이터, 각 패턴에 부합하는 어휘 항목을 가지고 있는다
 */
typedef struct token Token;

/**
 * @brief   토큰의 종류들을 모아놓은 열거형
 */
enum TOKEN_TYPE {
    TokenNone,
    TokenComma,
    TokenAssign,
    TokenColon,
    TokenEqual, TokenNotEqual,
    TokenPlus, TokenMinus,
    TokenAbsterisk /* '*' */, TokenSlash /* / */, TokenModulus /* % */,
    TokenOpenBracket /* [ */, TokenCloseBracket /* ] */,
    TokenIdentifier, TokenIntegerConstant, TokenFPConstant /* floating point */, TokenStringConstant,
    TokenBoolConstant, TokenCharConstant,
    TokenSemicolon /* ; */,
    TokenLeftBrace /* { */, TokenRightBrace /* } */,
    TokenLeftParen, TokenRightParen,
    TokenAutoType, TokenBoolType, TokenCharType, TokenIntType, TokenDoubleType,
    TokenVarDecl,
    TokenEndOfFile, TokenEndOfLine, TokenEndOfFunction,
};

/**
 * @brief   토큰종류와 속성값으로 구성되는 데이터, 각 패턴에 부합하는 어휘 항목을 가지고 있는다
 */
struct token {
    /*  @brief  토큰의 종류 */
    TOKEN_TYPE type;

    /*  @brief  토큰이 위치하는 줄 */
    unsigned int line;

    /*  @brief  토큰의 값 */
    union {
        u16char u16;
        int i32;
        double f64;
    };

    /*  @brief  토큰의 문자열 값 */
    union {
        u16char *str;
        wchar_t *dbg;
    };
};

/**
 * @brief   토큰의 종류와 라인 값으로 토큰을 만든다
 * @param   type    토큰의 종류
 * @param   line    토큰의 라인
 * @return  초기화된 토큰
 */
DLL_EXPORT Token TokenCreate(TOKEN_TYPE type, unsigned int line);

/**
 * @brief   토큰을 파괴한다
 * @param   token[in]   파괴할 토큰
 */
DLL_EXPORT void TokenDestroy(Token *token);

#endif //BOM_TOKEN_H
