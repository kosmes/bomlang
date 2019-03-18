//
// Created by jiney on 2019-03-17.
//

#include "vm.h"
#include "buf.h"
#include "error.h"

void init_vm(vm_t *vm) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        vm->full_mem[i] = 0;
    }

    vm->reg[PROGRAM_CODE] = 0;
    vm->reg[FRAME_POINTER] = 0;
    vm->reg[STACK_POINTER] = 0;

    vm->data = NULL;
    vm->text = NULL;
}

void final_vm(vm_t *vm) {
    buf_free(vm->data);
    buf_free(vm->text);
}

void set_script(vm_t *vm, script_t *script) {
    for (int i = 0; i < buf_len(script->data); i++) {
        buf_push(vm->data, script->data[i]);
    }

    for (int i = 0; i < buf_len(script->text); i++) {
        buf_push(vm->text, script->text[i]);
    }
}

#define NEXT_CODE(vm) ((vm)->text[(vm)->reg[PROGRAM_CODE]++])

#define CHECK_STACK_OVERFLOW(vm, offset) if ((vm)->reg[STACK_POINTER] + offset >= STACK_SIZE) \
    { error(STACK_OVERFLOW, 0); }

#define CHECK_STACK_UNDERFLOW(vm, offset) if ((vm)->reg[STACK_POINTER] - offset < 0) \
    { error(STACK_UNDERFLOW, 0); }

static void push_int(vm_t *vm, int data) {
    CHECK_STACK_OVERFLOW(vm, 4);

    converter_t cvt;
    cvt.asInteger = data;

    for(int i = 0; i < 4; i++) {
        vm->stack[vm->reg[STACK_POINTER]++] = cvt.asBytes[i];
    }
}

static void push_double(vm_t *vm, double data) {
    CHECK_STACK_OVERFLOW(vm, 8);

    converter_t cvt;
    cvt.asDouble = data;

    for(int i = 0; i < 8; i++) {
        vm->stack[vm->reg[STACK_POINTER]++] = cvt.asBytes[i];
    }
}

static int pop_int(vm_t *vm) {
    CHECK_STACK_UNDERFLOW(vm, 4);

    short stack_point = vm->reg[STACK_POINTER];
    converter_t cvt;
    cvt.asDouble = 0;

    for(int i = 0; i < 4; i++) {
        cvt.asBytes[3 - i] = vm->stack[--vm->reg[STACK_POINTER]];
    }

    return cvt.asInteger;
}

static double pop_double(vm_t *vm) {
    CHECK_STACK_UNDERFLOW(vm, 8);

    short stack_point = vm->reg[STACK_POINTER];
    converter_t cvt;

    for(int i = 0; i < 8; i++) {
        cvt.asBytes[7 - i] = vm->stack[--vm->reg[STACK_POINTER]];
    }

    return cvt.asInteger;
}

static void op_const(vm_t *vm) {
    type_code_t code = (type_code_t) NEXT_CODE(vm);

    converter_t cvt;
    cvt.asDouble = 0;
    for (int i = 0; i < 8; i++) {
        cvt.asBytes[i] = NEXT_CODE(vm);
    }

    if (code == TYPE_INT) {
        int data = get_int_from_addr(&vm->script, cvt.asSize);
        push_int(vm, data);
    } else if (code == TYPE_DOUBLE) {
        double data = get_double_from_addr(&vm->script, cvt.asSize);
        push_double(vm, data);
    }
}

static void op_add(vm_t *vm) {
    type_code_t code = (type_code_t) NEXT_CODE(vm);

    switch (code) {
        case TYPE_INT: {
            int rhs = pop_int(vm);
            int lhs = pop_int(vm);
            push_int(vm, lhs + rhs);
        } break;
        case TYPE_DOUBLE: {
            double rhs = pop_double(vm);
            double lhs = pop_double(vm);
            push_double(vm, lhs + rhs);
        } break;
        default:
            break;
    }
}

static void op_sub(vm_t *vm) {
    type_code_t code = (type_code_t) NEXT_CODE(vm);

    switch (code) {
        case TYPE_INT: {
            int rhs = pop_int(vm);
            int lhs = pop_int(vm);
            push_int(vm, lhs - rhs);
        } break;
        case TYPE_DOUBLE: {
            double rhs = pop_double(vm);
            double lhs = pop_double(vm);
            push_double(vm, lhs - rhs);
        } break;
        default:
            break;
    }
}

static void op_mul(vm_t *vm) {
    type_code_t code = (type_code_t) NEXT_CODE(vm);

    switch (code) {
        case TYPE_INT: {
            int rhs = pop_int(vm);
            int lhs = pop_int(vm);
            push_int(vm, lhs * rhs);
        } break;
        case TYPE_DOUBLE: {
            double rhs = pop_double(vm);
            double lhs = pop_double(vm);
            push_double(vm, lhs * rhs);
        } break;
        default:
            break;
    }
}

static void op_div(vm_t *vm) {
    type_code_t code = (type_code_t) NEXT_CODE(vm);

    switch (code) {
        case TYPE_INT: {
            int rhs = pop_int(vm);
            int lhs = pop_int(vm);
            push_int(vm, lhs / rhs);
        } break;
        case TYPE_DOUBLE: {
            double rhs = pop_double(vm);
            double lhs = pop_double(vm);
            push_double(vm, lhs / rhs);
        } break;
        default:
            break;
    }
}

static void op_cast(vm_t *vm) {
    type_code_t from_type = (type_code_t) NEXT_CODE(vm);
    type_code_t to_type = (type_code_t) NEXT_CODE(vm);

    if (from_type == TYPE_INT) {
        int data = pop_int(vm);
        switch (to_type) {
            case TYPE_DOUBLE:
                push_double(vm, (double) data);
                break;
            default:
                break;
        }
    } else if (from_type == TYPE_DOUBLE) {
        double data = pop_double(vm);
        switch (to_type) {
            case TYPE_INT:
                push_int(vm, (int) data);
                break;
            default:
                break;
        }
    }
}

static void op_invert(vm_t *vm) {
    type_code_t from_type = (type_code_t) NEXT_CODE(vm);

    if (from_type == TYPE_INT) {
        int data = pop_int(vm);
        push_int(vm, -data);
    } else if (from_type == TYPE_DOUBLE) {
        double data = pop_double(vm);
        push_int(vm, -data);
    }
}

static void op_dbg_print(vm_t *vm) {
    type_code_t code = (type_code_t) NEXT_CODE(vm);
    switch (code) {
        case TYPE_INT:
            wprintf(L"result: %d\n", pop_int(vm));
            break;
        case TYPE_DOUBLE:
            wprintf(L"result: %f\n", pop_double(vm));
            break;
    }
}

void run_vm(vm_t *vm) {
    bool running = true;

    do {
        char opcode = NEXT_CODE(vm);
        switch(opcode) {
            case OP_HALT:
                wprintf(L"program halted\n");
                running = false;
                break;

            case OP_CONST:
                op_const(vm);
                break;

            case OP_ADD:
                op_add(vm);
                break;

            case OP_SUB:
                op_sub(vm);
                break;

            case OP_MUL:
                op_mul(vm);
                break;

            case OP_DIV:
                op_div(vm);
                break;

            case OP_CAST:
                op_cast(vm);
                break;

            case OP_INVERT:
                op_invert(vm);
                break;

            case OP_DBG_PRNIT:
                op_dbg_print(vm);
                break;

            default:
                running = false;
                break;
        }
    } while(running);
}