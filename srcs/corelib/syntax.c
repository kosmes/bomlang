//
// Created by jiney on 2019-03-17.
//

#include "syntax.h"

#define WNULL L'\0'

typedef struct syntax_ syntax_t;

struct syntax_ {
    wchar_t *text;
    size_t text_len;

    wchar_t ch;
    size_t pos;

    unsigned int line;
};

syntax_t this;

void advance() {
    this.pos += 1;
    if (this.pos >= this.text_len) {
        this.ch = WNULL;
    } else {
        this.ch = this.text[this.pos];
        if (this.ch == L'\n') {
            this.line++;
        }
    }
}

token_t* get_tokens(wchar_t *text) {
    this.text = text;
    this.text_len = wcslen(text);

    this.pos = 0;
    this.ch = this.text[this.pos];

    this.line = 0;

    token_t *tokens = (token_t *)malloc(sizeof(token_t) * 128);

    while (this.ch != WNULL) {
        if (iswspace(this.ch)) {
            advance();
        }

        if (this.ch == L';') {
            create_token(TokenSemicolon, this.line);
        }
    }

    return tokens;
}

