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

void print_node(node_t *node, int indent) {
    if (node == NULL) {
        return;
    }

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

        case NodeVarDecl:
            wprintf(L"NodeVarDecl\n");
            break;

        case NodeEmpty:
            wprintf(L"NodeEmpty");
            break;

        case NodeType:
            wprintf(L"NodeType");
            switch (node->token.type) {
                case TokenAutoType:
                    wprintf(L" \t형: 자동 \n");
                    break;

                case TokenIntType:
                    wprintf(L" \t형: 정수 \n");
                    break;

                case TokenDoubleType:
                    wprintf(L" \t형: 실수 \n");
                    break;

                default:
                    wprintf(L"\n");
                    break;
            }
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

void print_const(script_t *script, size_t *offset) {
    converter_t cvt;
    cvt.asDouble = 0;

    for (int i = 0; i < 8; i++) {
        cvt.as_bytes[i] = script->text[(*(offset))++];
    }

    size_t addr = cvt.as_size;
    TYPE_IDS type_id = script->data[addr++];
    switch(type_id) {
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
                wprintf(L"const ");
                print_const(script, &offset);
                wprintf(L"\n");
            } break;
            case OP_STORE: {
                wprintf(L"store ");
                converter_t cvt;
                cvt.asDouble = 0;
                for (int i = 0; i < 8; i++) {
                    cvt.as_bytes[i] = fetch();
                }

                wprintf(L"\tto: %zd\n", cvt.as_size);
            } break;
            case OP_LOAD: {
                wprintf(L"load ");
                converter_t cvt;
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

    compiler_t compiler;
    vm_t vm;

    init_compiler(&compiler);
    init_vm(&vm);

    while (true) {
        reset_error_count();
        
        u16char *input = L"ㄱ=10 선언;";//readline(L"bom> ");
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

// #if DEBUG_MODE
        print_node(root_node, 0);
// #endif

        return 0;

        if (!compile(&compiler, root_node)) {
            continue;
        }

        destroy_node(root_node);

#if DEBUG_MODE
        print_script(compiler.root_script);
#endif

        set_script(&vm, compiler.root_script);
        final_script(compiler.root_script);

        run_vm(&vm);
    }

    final_compiler(&compiler);
    final_vm(&vm);
    return 0;
}
