//
// Created by jiney on 2019-03-17.
//

#include "syntax.h"
#include "buf.h"
#include "error.h"

#define WNULL L'\0'
#define CHECK1(c, t) if (this.ch == L##c) { buf_push(tokens, TokenCreate(t, this.line)); advance(); continue; }
#define CHECK2(c1, t1, c2, t2) if (this.ch == L##c1) \
    { buf_push(tokens, TokenCreate(t1, this.line)); advance(); continue; } \
    else if (this.ch == L##c2) { buf_push(tokens, TokenCreate(t2, this.line)); advance(); continue; }

typedef struct syntax Syntax;

struct syntax {
    u16char *text;
    size_t textLen;

    u16char ch;
    size_t pos;

    unsigned int line;
};

static Syntax this;

struct keyword {
    const u16char *key;
    const Token token;
};

static const unsigned char keyword_count = 3;
const struct keyword keywords[] = {
        {.key = L"선언", .token = {.type=TokenVarDecl,    .i32 = 0, .line = 0, .str = NULL}},
        {.key = L"자동", .token = {.type=TokenAutoType,   .i32 = 0, .line = 0, .str = NULL}},
        {.key = L"정수", .token = {.type=TokenIntType,    .i32 = 0, .line = 0, .str = NULL}},
        {.key = L"실수", .token = {.type=TokenDoubleType, .i32 = 0, .line = 0, .str = NULL}},
};

static bool isIdable(u16char ch) {
    const u16char *specials = L"!@#$%^&*()+=-<>,./?'\";:[]{}~`";
    u16char data[2] = { WNULL, };
    data[0] = ch;
    return (wcsstr(specials, data) == NULL);
}

static void advance() {
    this.pos += 1;
    if (this.pos >= this.textLen) {
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
    if (peek_pos > this.textLen) {
        return WNULL;
    } else {
        return this.text[peek_pos];
    }
}

static Token getNumber() {
    wchar_t buf[16];
    char offset = 0;
    Token token;

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

        token = TokenCreate(TokenFPConstant, this.line);
        token.f64 = (double) wcstod(buf, NULL);

    } else {
        token = TokenCreate(TokenIntegerConstant, this.line);
        token.i32 = (int) wcstol(buf, NULL, 10);
    }

    return token;
}

static Token getId() {
    u16char *start = NULL;
    size_t offset = 0;
    Token token;

    start = (this.text + this.pos);

    while (this.ch != WNULL && isIdable(this.ch) && !iswspace(this.ch)) {
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

static void skipWhitespace() {
    while (this.ch != WNULL && iswspace(this.ch)) {
        advance();
    }
}

Token *SyntaxGetTokens(wchar_t *text) {
    this.text = text;
    this.textLen = wcslen(text);

    this.pos = 0;
    this.ch = this.text[this.pos];

    this.line = 1;

    Token *tokens = NULL;

    while (this.ch != WNULL) {
        if (iswspace(this.ch)) {
            skipWhitespace();
            continue;
        }

        if (iswdigit(this.ch)) {
            buf_push(tokens, getNumber());
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

        if (isIdable(this.ch)) {
            buf_push(tokens, getId());
            continue;
        }

        ErrorLine(ERR_INVALID_TOKEN, this.line);
        advance();
    }

    buf_push(tokens, TokenCreate(TokenEndOfFile, this.line));

    if (ErrorGetCount() != 0) {
        buf_free(tokens);
        return NULL;
    }

    return tokens;
}

