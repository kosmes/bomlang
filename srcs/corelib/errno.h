//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_ERRNO_H
#define BOM_ERRNO_H

#include "common.h"

enum errco {
    SYNTAX_ERR,
    NO_EXPR,
    SEMI_EXPECTED,
    UNBAL_PARENS,
};

const wchar_t *errstr[] = {
        L"구문 오류.",
        L"식이 없습니다.",
        L"';'가 필요합니다.",
        L"'(' ')'가 올바르게 연결되지 않았습니다.",
};

/**
 * 주어진 에러 코드로 에러를 출력한다
 * @param err   출력할 에러 코드
 * @param line  에러가 난 줄
 */
DLL_EXPORT void error(enum errco err, unsigned int line);

#endif //BOM_ERRNO_H
