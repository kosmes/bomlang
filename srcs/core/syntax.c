﻿//
// Created by jiney on 2019-03-17.
//

#include "syntax.h"
#include "buf.h"
#include "error.h"

#define WNULL L'\0'
#define CHECK1(c, t) if (this.ch == L##c) { buf_push(tokens, create_token(t, this.line)); advance(); continue; }
#define CHECK2(c1, t1, c2, t2) if (this.ch == L##c1) \
    { buf_push(tokens, create_token(t1, this.line)); advance(); continue; } \
    else if (this.ch == L##c2) { buf_push(tokens, create_token(t2, this.line)); advance(); continue; }

typedef struct syntax syntax_t;

struct syntax {
    u16char *text;
    size_t text_len;

    u16char ch;
    size_t pos;

    unsigned int line;
};

static syntax_t this;

struct keyword {
    const u16char *key;
    const token_t token;
};

static const unsigned char keyword_count = 3;
const struct keyword keywords[] = {
        {.key = L"선언", .token = {.type=TokenVarDecl,    .i32 = 0, .line = 0, .str = NULL}},
        {.key = L"자동", .token = {.type=TokenAutoType,   .i32 = 0, .line = 0, .str = NULL}},
        {.key = L"정수", .token = {.type=TokenIntType,    .i32 = 0, .line = 0, .str = NULL}},
        {.key = L"실수", .token = {.type=TokenDoubleType, .i32 = 0, .line = 0, .str = NULL}},
};

static bool is_idable(u16char ch) {
    const u16char *specials = L"!@#$%^&*()+=-<>,./?'\";:[]{}~`";
    u16char data[2] = { WNULL, };
    data[0] = ch;
    return (wcsstr(specials, data) == NULL);
}

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

static u16char peek() {
    size_t peek_pos = this.pos + 1;
    if (peek_pos > this.text_len) {
        return WNULL;
    } else {
        return this.text[peek_pos];
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
        token.f64 = (double) wcstod(buf, NULL);

    } else {
        token = create_token(TokenIntegerConstant, this.line);
        token.i32 = (int) wcstol(buf, NULL, 10);
    }

    return token;
}

static token_t get_id() {
    u16char *start = NULL;
    size_t offset = 0;
    token_t token;

    start = (this.text + this.pos);

    while (this.ch != WNULL && is_idable(this.ch) && !iswspace(this.ch)) {
        offset++;
        advance();
    }

    u16char *id = malloc(sizeof(u16char) * (offset + 1));
    wcsncpy(id, start, offset);

    id[offset] = WNULL;

    for (int i = 0; i < keyword_count; i++) {
        if (wcscmp(keywords[i].key, id) == 0) {
            token = keywords[i].token;
            token.line = this.line;

            return token;
        }
    }

    token.type = TokenIdentifier;
    token.i32 = 0;
    token.line = this.line;
    token.str = id;

    return token;
}

static void skip_whitespace() {
    while (this.ch != WNULL && iswspace(this.ch)) {
        advance();
    }
}

token_t *get_tokens(wchar_t *text) {
    this.text = text;
    this.text_len = wcslen(text);

    this.pos = 0;
    this.ch = this.text[this.pos];

    this.line = 1;

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

        if (is_idable(this.ch)) {
            buf_push(tokens, get_id());
            continue;
        }

        error_line(ERR_INVALID_TOKEN, this.line);
        advance();
    }

    buf_push(tokens, create_token(TokenEndOfFile, this.line));

    if (get_error_count() != 0) {
        buf_free(tokens);
        return NULL;
    }

    return tokens;
}

