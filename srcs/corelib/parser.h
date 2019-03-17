//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_PARSER_H
#define BOM_PARSER_H

#include "common.h"
#include "node.h"
#include "token.h"

DLL_EXPORT node_t *do_parse(token_t *tokens);

#endif //BOM_PARSER_H
