//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_ERRNO_H
#define BOM_ERRNO_H

#include "common.h"

enum ERROR_CODE {
    ERR_ERROR,
    ERR_SYNTAX,
    ERR_NO_EXPR,
    ERR_SEMI_EXPECTED,
    ERR_TYPE_EXPECTED,
    ERR_UNBAL_PARENS,
    ERR_STACK_OVERFLOW,
    ERR_STACK_UNDERFLOW,
    ERR_DIVIDE_BY_ZERO,
    ERR_INVALID_EOF,
    ERR_INVALID_INDEX,
    ERR_INVALID_ACCESS,
    ERR_INVALID_TOKEN,
    ERR_UNDEF_VAR,
};

/**
 * @brief   주어진 에러 코드로 에러를 출력한다
 * @param   err     출력할 에러 코드
 */
DLL_EXPORT void error(enum ERROR_CODE err);

/**
 * @brief   주어진 에러 코드와 줄로 에러를 출력한다
 * @param   err     출력할 에러 코드
 * @param   line    에러가 난 줄
 */
DLL_EXPORT void error_line(enum ERROR_CODE err, unsigned int line);

/**
 * @brief   에러가 발생한 횟수를 초기화한다
 */
DLL_EXPORT void reset_error_count();

/**
 * @brief   에러가 발생한 횟수를 반환한다
 * @return  에러가 발생한 횟수
 */
DLL_EXPORT unsigned short get_error_count();

#endif //BOM_ERRNO_H
