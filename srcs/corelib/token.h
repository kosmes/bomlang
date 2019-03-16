//
// Created by jiney on 2019-03-17.
//

#ifndef BINARY_TOKEN_H
#define BINARY_TOKEN_H

typedef enum token_type token_type_t;

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

#endif //BINARY_TOKEN_H
