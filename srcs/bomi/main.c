#include <locale.h>

#include "parser.h"
#include "syntax.h"
#include "buf.h"
#include "compiler.h"
#include "runtime.h"
#include "vm.h"

#include "table.h"

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

void print_node(node_t *node, int indent) {
    for (int i = 0; i < indent; i++) {
        wprintf(L"\t");
    }

    wprintf(L"노드: ");
    switch(node->type) {
        case NodeIntegerConstant:
            wprintf(L"NodeIntegerConstant");
            wprintf(L" 정수 값: %d\n", node->token.i32);
            break;

        case NodeFPConstant:
            wprintf(L"NodeFPConstant");
            wprintf(L" 실수 값: %lf\n", node->token.f64);
            break;

        case NodeAssignOp:
            wprintf(L"NodeAssignOp\n");
            break;

        case NodeCompound:
            wprintf(L"NodeCompound\n");
            break;

        case NodeUnaryOp:
            wprintf(L"NodeUnaryOp\n");
            break;

        case NodeVar:
            wprintf(L"NodeVar");
            wprintf(L" 변수 값 : %ls\n", node->token.str);
            break;

        case NodeEmpty:
            wprintf(L"NodeEmpty");
            break;

        default:
            wprintf(L"\n");
            break;
    }

    for(int i = 0; i < buf_len(node->child); i++) {
        print_node(node->child[i], indent + 1);
    }
}

#define fetch() script->text[offset++]

void print_type(TYPE_IDS code) {
    const u16char *str[] = {
            L"none",
            L"integer",
            L"double"
    };

    wprintf(L"\ttype: %7ls ", str[(char) code]);
}

void print_const(script_t *script, TYPE_IDS code, size_t *offset) {
    converter_t cvt;
    cvt.asDouble = 0;

    for (int i = 0; i < 8; i++) {
        cvt.asBytes[i] = script->text[(*(offset))++];
    }

    size_t addr = cvt.asSize;
    switch(code) {
        case TYPE_INT: {
            int data = get_int_from_addr(script, addr);
            wprintf(L"\tvalue: %d ", data);
        } break;
        case TYPE_DOUBLE: {
            double data = get_double_from_addr(script, addr);
            wprintf(L"\tvalue: %f ", data);
        } break;
        default:
            break;
    }
}

void print_script(script_t *script) {
    size_t len = buf_len(script->text);
    size_t offset = 0;
    while(offset < len) {
        switch (fetch()) {
            case OP_CONST: {
                TYPE_IDS type = fetch();
                wprintf(L"const ");
                print_type(type);
                print_const(script, type, &offset);
                wprintf(L"\n");
            } break;
            case OP_STORE: {
                TYPE_IDS type = fetch();
                wprintf(L"store ");
                print_type(type);
                converter_t cvt;
                cvt.asDouble = 0;
                for (int i = 0; i < 8; i++) {
                    cvt.asBytes[i] = fetch();
                }

                wprintf(L"\tto: %zd\n", cvt.asSize);
            } break;
            case OP_LOAD: {
                TYPE_IDS type = fetch();
                wprintf(L"load ");
                print_type(type);
                converter_t cvt;
                cvt.asDouble = 0;
                for (int i = 0; i < 8; i++) {
                    cvt.asBytes[i] = fetch();
                }

                wprintf(L"\tfrom: %zd\n", cvt.asSize);
            } break;
            case OP_ADD: {
                wprintf(L"add ");
                print_type(fetch());
                wprintf(L"\n");
            } break;
            case OP_SUB: {
                wprintf(L"sub ");
                print_type(fetch());
                wprintf(L"\n");
            } break;
            case OP_MUL: {
                wprintf(L"mul ");
                print_type(fetch());
                wprintf(L"\n");
            } break;
            case OP_DIV: {
                wprintf(L"div ");
                print_type(fetch());
                wprintf(L"\n");
            } break;
            case OP_CAST: {
                wprintf(L"cast ");
                print_type(fetch());
                print_type(fetch());
                wprintf(L"\n");
            } break;
            case OP_INVERT: {
                wprintf(L"invert ");
                print_type(fetch());
                wprintf(L"\n");
            } break;
        }
    }
}

int main(void) {
    setlocale(LC_ALL, "");

    wprintf(L"봄 인터프리터 0.0.1 베타 버전\n");
    wprintf(L"종료하려면 Ctrl+C를 누르거나 '종료'를 입력하세요.\n");

    while (true) {
        u16char *input = L"ㄱ = 10 + 1.2;\n ㄴ = 10 * ㄱ;"; //readline(L"bom> ");
        if(wcscmp(input, L"종료") == 0) {
            break;
        }

        token_t *tokens = get_tokens(input);
        if (tokens == NULL) {
            continue;
        }

        node_t *root_node = do_parse(tokens);

        if (root_node == NULL) {
            continue;
        }

        // print_node(root_node, 0);

        script_t *script = compile(root_node);
        print_script(script);

        return 0;

        vm_t vm;
        init_vm(&vm);

        set_script(&vm, script);
        final_script(script);

        run_vm(&vm);
        final_vm(&vm);
    }
    return 0;
}
