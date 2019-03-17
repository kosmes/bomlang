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

void add_int_and_put_addr(script_t *script, int data) {
    converter_t cvt;
    cvt.asInteger = data;

    size_t addr = buf_len(script->data);

    for (int i = 0; i < 4; i++) {
        buf_push(script->data, cvt.asBytes[0]);
    }

    cvt.asSize = addr;

    for (int i = 0; i < 8; i++) {
        buf_push(script->text, cvt.asBytes[i]);
    }
}

void add_double_and_put_addr(script_t *script, double data) {
    converter_t cvt;
    cvt.asDouble = data;

    size_t addr = buf_len(script->data);

    for (int i = 0; i < 8; i++) {
        buf_push(script->data, cvt.asBytes[0]);
    }

    cvt.asSize = addr;

    for (int i = 0; i < 8; i++) {
        buf_push(script->text, cvt.asBytes[i]);
    }
}

void add_string_and_put_addr(script_t *script, const wchar_t *data) {
    short len = (short) wcslen(data);
    size_t addr = (size_t) buf_len(script->data);

    converter_t cvt;
    cvt.asInteger = len;

    for (int i = 0; i < 2; i++) {
        buf_push(script->data, cvt.asBytes[0]);
    }

    for (int i = 0; i < len; i++) {
        cvt.asChar = data[i];
        buf_push(script->data, cvt.asBytes[0]);
        buf_push(script->data, cvt.asBytes[1]);
    }

    cvt.asSize = addr;

    for (int i = 0; i < 8; i++) {
        buf_push(script->text, cvt.asBytes[i]);
    }
}
