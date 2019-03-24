//
// Created by jiney on 2019-03-17.
//

#include "script.h"
#include "container/buf.h"
#include "runtime.h"

void ScriptDestroy(void *ptr) {
    Script *script = (Script *) ptr;

    buf_free(script->data);
    buf_free(script->text);

    script->text = NULL;
    script->data = NULL;
}

Script *ScriptCreate() {
    Script *script = new (Script, ScriptDestroy);

    script->text = NULL;
    script->data = NULL;

    return script;
}

size_t ScriptAddIntAndReturnAddr(Script *script, int data) {
    Converter cvt;
    cvt.as_integer = data;

    size_t addr = buf_len(script->data);

    buf_push(script->data, TYPE_INT);

    for (int i = 0; i < 4; i++) {
        buf_push(script->data, cvt.as_bytes[i]);
    }

    return addr;
}

size_t ScriptAddDoubleAndReturnAddr(Script *script, double data) {
    Converter cvt;
    cvt.asDouble = data;

    size_t addr = buf_len(script->data);

    buf_push(script->data, TYPE_DOUBLE);

    for (int i = 0; i < 8; i++) {
        buf_push(script->data, cvt.as_bytes[i]);
    }

    return addr;
}

size_t ScriptAddStringAndReturnAddr(Script *script, const wchar_t *data) {
    short len = (short) wcslen(data);
    size_t addr = (size_t) buf_len(script->data);

    Converter cvt;
    cvt.as_integer = len;

    // buf_push(script->data, TYPE_STRING);

    for (int i = 0; i < 2; i++) {
        buf_push(script->data, cvt.as_bytes[i]);
    }

    for (int i = 0; i < len; i++) {
        cvt.as_char = data[i];
        buf_push(script->data, cvt.as_bytes[0]);
        buf_push(script->data, cvt.as_bytes[1]);
    }

    return addr;
}

int ScriptGetIntFromAddr(Script *script, size_t addr) {
    size_t offset = addr;
    Converter cvt;
    cvt.asDouble = 0;

    for (int i = 0; i < 4; i++) {
        cvt.as_bytes[i] = script->data[offset + i];
    }

    return cvt.as_integer;
}

double ScriptGetDoubleFromAddr(Script *script, size_t addr) {
    size_t offset = addr;
    Converter cvt;
    cvt.asDouble = 0;

    for (int i = 0; i < 8; i++) {
        cvt.as_bytes[i] = script->data[offset + i];
    }

    return cvt.asDouble;
}

const wchar_t *ScriptGetStringFromAddr(Script *script, size_t addr) {
    size_t offset = addr;
    short len = 0;

    Converter cvt;
    cvt.asDouble = 0;

    for (int i = 0; i < 2; i++) {
        cvt.as_bytes[i] = script->data[offset++];
    }

    len = cvt.as_short;
    wchar_t *str = malloc(sizeof(wchar_t) * (len + 1));

    for (int i = 0; i < len; i++) {
        cvt.asDouble = 0;

        cvt.as_bytes[0] = script->data[offset++];
        cvt.as_bytes[1] = script->data[offset++];

        str[i] = cvt.as_char;
    }

    str[len] = L'\0';

    return str;
}

#define fetch() script->text[offset++]

void printConst(Script *script, size_t *offset) {
    Converter cvt;
    cvt.asDouble = 0;

    for (int i = 0; i < 8; i++) {
        cvt.as_bytes[i] = script->text[(*(offset))++];
    }

    size_t addr = cvt.as_size;
    TYPE_ID type_id = script->data[addr++];
    switch(type_id) {
        case TYPE_INT: {
            int data = ScriptGetIntFromAddr(script, addr);
            wprintf(L"\tvalue: %d ", data);
        } break;
        case TYPE_DOUBLE: {
            double data = ScriptGetDoubleFromAddr(script, addr);
            wprintf(L"\tvalue: %f ", data);
        } break;
        default:
            break;
    }
}

void ScriptPrint(Script *script) {
    size_t len = buf_len(script->text);
    size_t offset = 0;
    while(offset < len) {
        switch (fetch()) {
            case OP_CONST: {
                wprintf(L"const ");
                printConst(script, &offset);
                wprintf(L"\n");
            } break;
            case OP_STORE: {
                wprintf(L"store ");
                Converter cvt;
                cvt.asDouble = 0;
                for (int i = 0; i < 8; i++) {
                    cvt.as_bytes[i] = fetch();
                }

                wprintf(L"\tto: %zd\n", cvt.as_size);
            } break;
            case OP_LOAD: {
                wprintf(L"load ");
                Converter cvt;
                cvt.asDouble = 0;
                for (int i = 0; i < 8; i++) {
                    cvt.as_bytes[i] = fetch();
                }

                wprintf(L"\tfrom: %zd\n", cvt.as_size);
            } break;
            case OP_ADD: {
                wprintf(L"add ");
                wprintf(L"\n");
            } break;
            case OP_SUB: {
                wprintf(L"sub ");
                wprintf(L"\n");
            } break;
            case OP_MUL: {
                wprintf(L"mul ");
                wprintf(L"\n");
            } break;
            case OP_DIV: {
                wprintf(L"div ");
                wprintf(L"\n");
            } break;
            case OP_INVERT: {
                wprintf(L"invert ");
                wprintf(L"\n");
            } break;
        }
    }
}