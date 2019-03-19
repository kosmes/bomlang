﻿//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_ERRNO_H
#define BOM_ERRNO_H

#include "common.h"

enum ERROR_CODE {
    ERR_SYNTAX,
    ERR_NO_EXPR,
    ERR_SEMI_EXPECTED,
    ERR_UNBAL_PARENS,
    ERR_STACK_OVERFLOW,
    ERR_STACK_UNDERFLOW,
    ERR_DIVIDE_BY_ZERO,
    ERR_INVALID_EOF,
    ERR_INVALID_INDEX,
};

DLL_EXPORT void error(enum ERROR_CODE err);

/**
 * 주어진 에러 코드로 에러를 출력한다
 * @param err   출력할 에러 코드
 * @param line  에러가 난 줄
 */
DLL_EXPORT void error_line(enum ERROR_CODE err, unsigned int line);

#endif //BOM_ERRNO_H
