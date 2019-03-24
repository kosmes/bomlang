//
// Created by jiney on 2019-03-23.
//

#ifndef BOM_SYMBOLTABLE_H
#define BOM_SYMBOLTABLE_H

#include "common.h"
#include "container/table.h"
#include "symbol/symbol.h"

typedef struct symbol_table SymbolTable;

struct symbol_table {
    SymbolTable *parent;

    Table *symbols;
};

DLL_EXPORT SymbolTable *SymbolTableCreate(SymbolTable *parent);

DLL_EXPORT void SymbolTableDefine(SymbolTable *table, Symbol *symbol);

DLL_EXPORT Symbol *SymbolTableLookUp(SymbolTable *table, const u16char *name);

#endif //BOM_SYMBOLTABLE_H