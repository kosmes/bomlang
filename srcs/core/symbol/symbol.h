//
// Created by jiney on 2019-03-23.
//

#ifndef BOM_SYMBOL_H
#define BOM_SYMBOL_H

#include "common.h"

typedef struct symbol Symbol;

enum SYMBOL_TYPE {
    SYMBOL_TYPE_BUILTIN,
    SYMBOL_TYPE_VAR,
    SYMBOL_TYPE_FUNC,
};

struct symbol {
    unsigned char type;
    u16char *name;
    Symbol *base;
};

DLL_EXPORT Symbol *SymbolCreate(unsigned char type, const u16char *name, Symbol *base);

#endif //BOM_SYMBOL_H
