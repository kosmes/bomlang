//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_PARSER_H
#define BOM_PARSER_H

#include "common.h"
#include "node.h"
#include "token.h"

/**
 * @brief   주어진 토큰을 분석하여 ast 노드로 변환한다
 * @param   tokens  분석한 토큰들
 * @return  변환된 노드
 */
DLL_EXPORT node_t *do_parse(token_t *tokens);

#endif //BOM_PARSER_H
