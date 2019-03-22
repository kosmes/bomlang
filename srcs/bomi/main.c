#include <locale.h>

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

#define fetch() script->text[offset++]

void print_const(Script *script, size_t *offset) {
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

void print_script(Script *script) {
    size_t len = buf_len(script->text);
    size_t offset = 0;
    while(offset < len) {
        switch (fetch()) {
            case OP_CONST: {
                wprintf(L"const ");
                print_const(script, &offset);
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

int main(void) {
    setlocale(LC_ALL, "");

    wprintf(L"봄 인터프리터 0.0.2 베타 버전\n");
    wprintf(L"종료하려면 Ctrl+C를 누르거나 '종료'를 입력하세요.\n");

    Compiler compiler;
    vm_t vm;

    CompilerInit(&compiler);
    init_vm(&vm);

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

        node_t *root_node = ParserDoParse(tokens);

        if (root_node == NULL) {
            continue;
        }

#if DEBUG_MODE
        print_node(root_node, 0);
#endif

        if (!CompilerTryCompile(&compiler, root_node)) {
            continue;
        }

        NodeDestroy(root_node);

#if DEBUG_MODE
        print_script(compiler.root_script);
#endif

        set_script(&vm, compiler.root_script);
        ScriptFinal(compiler.root_script);

        run_vm(&vm);
    }

    CompilerFinal(&compiler);
    final_vm(&vm);

    return 0;
}
