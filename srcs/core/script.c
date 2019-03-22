//
// Created by jiney on 2019-03-17.
//

#include "script.h"
#include "buf.h"
#include "runtime.h"

void ScriptInit(Script *script) {
    script->text = NULL;
    script->data = NULL;
}

DLL_EXPORT void ScriptFinal(Script *script) {
    buf_free(script->data);
    buf_free(script->text);

    script->text = NULL;
    script->data = NULL;
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