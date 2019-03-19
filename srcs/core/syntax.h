//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_SYNTAX_H
#define BOM_SYNTAX_H

#include "common.h"
#include "token.h"

/**
 * 다음 토큰을 얻는다
 * @param text  토큰을 얻을 문자열
 * @return  토큰 리스트
 */
DLL_EXPORT token_t* get_tokens(wchar_t *text);

#endif //BOM_SYNTAX_H
