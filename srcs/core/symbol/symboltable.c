//
// Created by jiney on 2019-03-23.
//

#include "symbol/symboltable.h"

void SymbolTableDestroy(void *ptr) {
    SymbolTable *table = (SymbolTable *) ptr;

    _delete(table->symbols);
}

SymbolTable *SymbolTableCreate(SymbolTable *parent) {
    SymbolTable *table = create (SymbolTable, SymbolTableDestroy);

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