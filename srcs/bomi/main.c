#include <locale.h>

#include "syntax.h"
#include "buf.h"

static wchar_t buffer[2048];

/* Fake readline function */
wchar_t *readline(wchar_t *prompt) {
    fputws(prompt, stdout);
    fgetws(buffer, 2048, stdin);
    wchar_t* cpy = malloc((wcslen(buffer) + 1) * sizeof(wchar_t));
    wcscpy(cpy, buffer);
    cpy[wcslen(cpy)-1] = '\0';
    return cpy;
}

void add_history(char* unused) {}

int main(void) {
    setlocale(LC_ALL, "");

    wprintf(L"봄 프로그래밍 언어 0.0.1\n");
    wprintf(L"종료하려면 Ctrl+C를 누르거나 '종료'를 입력하세요.\n");

    while (true) {
        wchar_t *input = readline(L"bom> ");
        if(wcscmp(input, L"종료") == 0) {
            break;
        }

        token_t *tokens = get_tokens(input);
        for(int i = 0; i < buf_len(tokens); i++) {
            wprintf(L"토큰: %d", tokens[i].type);
            switch(tokens[i].type) {
                case TokenIntegerConstant:
                    wprintf(L" 정수 값: %d\n", tokens[i].i32);
                    break;

                case TokenFPConstant:
                    wprintf(L" 실수 값: %f\n", tokens[i].f64);
                    break;

                default:
                    wprintf(L"\n");
                    break;
            }
        }
    }
    return 0;
}
