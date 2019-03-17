//
// Created by jiney on 2019-03-17.
//

#include "script.h"
#include "buf.h"

typedef union converter converter_t;

union converter {
    short asShort;
    int asInteger;
    double asDouble;
    wchar_t asChar;
    size_t asSize;
    char asBytes[8];
};

void init_script(script_t *script) {
    script->text = NULL;
    script->data = NULL;
}

DLL_EXPORT void final_script(script_t *script) {
    buf_free(script->data);
    buf_free(script->text);
}

size_t add_int_and_return_addr(script_t *script, int data) {
    converter_t cvt;
    cvt.asInteger = data;

    size_t addr = buf_len(script->data);

    for (int i = 0; i < 4; i++) {
        buf_push(script->data, cvt.asBytes[0]);
    }

    return addr;
}

size_t add_double_and_return_addr(script_t *script, double data) {
    converter_t cvt;
    cvt.asDouble = data;

    size_t addr = buf_len(script->data);

    for (int i = 0; i < 8; i++) {
        buf_push(script->data, cvt.asBytes[0]);
    }

    return addr;
}

size_t add_string_and_return_addr(script_t *script, const wchar_t *data) {
    short len = (short) wcslen(data);
    size_t addr = (size_t) buf_len(script->data);

    converter_t cvt;
    cvt.asInteger = len;

    for (int i = 0; i < 2; i++) {
        buf_push(script->data, cvt.asBytes[i]);
    }

    for (int i = 0; i < len; i++) {
        cvt.asChar = data[i];
        buf_push(script->data, cvt.asBytes[0]);
        buf_push(script->data, cvt.asBytes[1]);
    }

    return addr;
}

int get_int_from_addr(script_t *script, size_t addr) {
    size_t offset = addr;
    converter_t cvt;
    cvt.asDouble = 0;

    for (int i = 0; i < 4; i++) {
        cvt.asBytes[i] = script->data[offset + i];
    }

    return cvt.asInteger;
}

double get_double_from_addr(script_t *script, size_t addr) {
    size_t offset = addr;
    converter_t cvt;
    cvt.asDouble = 0;

    for (int i = 0; i < 8; i++) {
        cvt.asBytes[i] = script->data[offset + i];
    }

    return cvt.asDouble;
}

const wchar_t *get_string_from_addr(script_t *script, size_t addr) {
    size_t offset = addr;
    short len = 0;

    converter_t cvt;
    cvt.asDouble = 0;

    for (int i = 0; i < 2; i++) {
        cvt.asBytes [i] = script->data[offset++];
    }

    len = cvt.asShort;
    wchar_t *str = malloc(sizeof(wchar_t) * (len + 1));

    for (int i = 0; i < len; i++) {
        cvt.asDouble = 0;

        cvt.asBytes[0] = script->data[offset++];
        cvt.asBytes[1] = script->data[offset++];

        str[i] = cvt.asChar;
    }

    str[len] = L'\0';

    return str;
}