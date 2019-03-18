//
// Created by jiney on 2019-03-17.
//

#include "syntax.h"
#include "buf.h"

#define WNULL L'\0'
#define CHECK1(c, t) if (this.ch == L##c) { buf_push(tokens, create_token(t, this.line)); advance(); continue; }
#define CHECK2(c1, t1, c2, t2) if (this.ch == L##c1) \
    { buf_push(tokens, create_token(t1, this.line)); advance(); continue; } \
    else if (this.ch == L##c2) { buf_push(tokens, create_token(t2, this.line)); advance(); continue; }

typedef struct syntax syntax_t;

struct syntax {
    wchar_t *text;
    size_t text_len;

    wchar_t ch;
    size_t pos;

    unsigned int line;
};

static syntax_t this;

static void advance() {
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

static token_t get_number() {
    wchar_t buf[16];
    char offset = 0;
    token_t token;

    while (this.ch != WNULL && iswdigit(this.ch)) {
        buf[offset++] = this.ch;
        advance();
    }

    if (this.ch == L'.') {
        buf[offset++] = this.ch;
        advance();

        while (this.ch != WNULL && iswdigit(this.ch)) {
            buf[offset++] = this.ch;
            advance();
        }

        token = create_token(TokenFPConstant, this.line);
        token.f64 = (double) wcstod(buf, WNULL);

    } else {
        token = create_token(TokenIntegerConstant, this.line);
        token.i32 = (int) wcstol(buf, WNULL, 10);
    }

    return token;
}

static void skip_whitespace() {
    while (this.ch != WNULL && iswspace(this.ch)) {
        advance();
    }
}

token_t* get_tokens(wchar_t *text) {
    this.text = text;
    this.text_len = wcslen(text);

    this.pos = 0;
    this.ch = this.text[this.pos];

    this.line = 0;

    token_t *tokens = NULL;

    while (this.ch != WNULL) {
        if (iswspace(this.ch)) {
            skip_whitespace();
            continue;
        }

        if (iswdigit(this.ch)) {
            buf_push(tokens, get_number());
            continue;
        }

        CHECK1(';', TokenSemicolon);
        CHECK1('(', TokenLeftParen);
        CHECK1(')', TokenRightParen);
        CHECK1('{', TokenLeftBrace);
        CHECK1('}', TokenRightBrace);
        CHECK1('=', TokenAssign);
        CHECK1(':', TokenColon);
        CHECK1(',', TokenComma);
        CHECK1('+', TokenPlus);
        CHECK1('-', TokenMinus);
        CHECK1('*', TokenAbsterisk);
        CHECK1('/', TokenSlash);
        CHECK1('%', TokenModulus);
    }

    return tokens;
}

