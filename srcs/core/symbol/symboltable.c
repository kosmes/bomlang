//
// Created by jiney on 2019-03-23.
//

#include "symbol/symboltable.h"

void SymbolTableDestroy(void *ptr) {
    SymbolTable *table = (SymbolTable *)table;

    delete(table->symbols);
}

SymbolTable *SymbolTableCreate(SymbolTable *parent) {
    SymbolTable *table = new (sizeof(SymbolTable), SymbolTableDestroy);

    table->symbols = TableCreate();
    table->parent = parent;

    return table;
}

void SymbolTableDefine(SymbolTable *table, Symbol *symbol) {
    TableSetData(table->symbols, symbol->name, symbol);
}

Symbol *SymbolTableLookUp(SymbolTable *table, const u16char *name) {
    TablePair *data = TableGetData(table->symbols, name);
    if (data == NULL) {
        return NULL;
    } else {
        return data->data;
    }
}