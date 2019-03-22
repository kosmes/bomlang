﻿#include <locale.h>

#include "parser.h"
#include "syntax.h"
#include "buf.h"
#include "compiler.h"
#include "runtime.h"
#include "vm.h"
#include "error.h"

static u16char buffer[2048];

/* Fake readline function */
u16char *readline(u16char *prompt) {
    fputws(prompt, stdout);
    fgetws(buffer, 2048, stdin);
    u16char* cpy = malloc((wcslen(buffer) + 1) * sizeof(u16char));
    wcscpy(cpy, buffer);
    cpy[wcslen(cpy)-1] = '\0';
    return cpy;
}

void add_history(u16char* unused) {}

int main(void) {
    setlocale(LC_ALL, "");

    wprintf(L"봄 인터프리터 0.0.2 베타 버전\n");
    wprintf(L"종료하려면 Ctrl+C를 누르거나 '종료'를 입력하세요.\n");

    Compiler compiler;
    Machine vm;

    CompilerInit(&compiler);
    MachineInit(&vm);

    while (true) {
        ErrorResetCount();
        
        u16char *input = readline(L"bom> ");
        if(wcscmp(input, L"종료") == 0) {
            break;
        }

        Token *tokens = SyntaxGetTokens(input);
        if (tokens == NULL) {
            continue;
        }

        Node *root_node = ParserDoParse(tokens);

        if (root_node == NULL) {
            continue;
        }

#if DEBUG_MODE
        NodePrint(root_node, 0);
#endif

        if (!CompilerTryCompile(&compiler, root_node)) {
            continue;
        }

        NodeDestroy(root_node);

#if DEBUG_MODE
        ScriptPrint(compiler.rootScript);
#endif

        MachineSetScript(&vm, compiler.rootScript);
        ScriptFinal(compiler.rootScript);

        MachineRun(&vm);
    }

    CompilerFinal(&compiler);
    MachineFinal(&vm);

    return 0;
}
