//
// Created by jiney on 2019-03-23.
//

#include "symbol/symbol.h"

void SymbolDestroy(void *ptr) {
    Symbol *symbol = (Symbol *) ptr;
    free(symbol->name);
}

Symbol *SymbolCreate(unsigned char type, const u16char *name, Symbol *base) {
    Symbol *symbol = new(sizeof(Symbol), SymbolDestroy);

    size_t name_size = sizeof(u16char) * (wcslen(name) + 1);

    symbol->type = type;

    symbol->name = malloc(name_size);
    memcpy(symbol->name, name, name_size);
    symbol->base = base;

    return symbol;
}