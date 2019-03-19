﻿//
// Created by jiney on 2019-03-17.
//

#include "vm.h"
#include "buf.h"
#include "error.h"

void init_vm(vm_t *vm) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        vm->full_mem[i] = 0;
    }

    vm->reg[REG_PROGRAM_CODE] = 0;
    vm->reg[REG_FRAME_POINTER] = 0;
    vm->reg[REG_STACK_POINTER] = 0;

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

#define NEXT_CODE(vm) ((vm)->text[(vm)->reg[REG_PROGRAM_CODE]++])
#define THROW_ERROR(vm, errcode) (error(errcode, 0), \
        (vm)->reg[REG_MACHINE_STATUS] = STATUS_ERROR_THROWN); return;

#define CHECK_STACK_OVERFLOW(vm, offset) if ((vm)->reg[REG_STACK_POINTER] + offset >= STACK_SIZE) \
    { error(ERR_STACK_OVERFLOW, 0); return; }

#define CHECK_STACK_UNDERFLOW(vm, offset) if ((vm)->reg[REG_STACK_POINTER] - offset < 0) \
    { error(ERR_STACK_UNDERFLOW, 0); return 0; }

static void push_int(vm_t *vm, int data) {
    CHECK_STACK_OVERFLOW(vm, 4);

    converter_t cvt;
    cvt.asInteger = data;

    for (int i = 0; i < 4; i++) {
        vm->stack[vm->reg[REG_STACK_POINTER]++] = cvt.asBytes[i];
    }
}

static void push_double(vm_t *vm, double data) {
    CHECK_STACK_OVERFLOW(vm, 8);

    converter_t cvt;
    cvt.asDouble = data;

    for (int i = 0; i < 8; i++) {
        vm->stack[vm->reg[REG_STACK_POINTER]++] = cvt.asBytes[i];
    }
}

static int pop_int(vm_t *vm) {
    CHECK_STACK_UNDERFLOW(vm, 4);

    short stack_point = vm->reg[REG_STACK_POINTER];
    converter_t cvt;
    cvt.asDouble = 0;

    for (int i = 0; i < 4; i++) {
        cvt.asBytes[3 - i] = vm->stack[--vm->reg[REG_STACK_POINTER]];
    }

    return cvt.asInteger;
}

static double pop_double(vm_t *vm) {
    CHECK_STACK_UNDERFLOW(vm, 8);

    short stack_point = vm->reg[REG_STACK_POINTER];
    converter_t cvt;

    for (int i = 0; i < 8; i++) {
        cvt.asBytes[7 - i] = vm->stack[--vm->reg[REG_STACK_POINTER]];
    }

    return cvt.asDouble;
}

static void op_const(vm_t *vm) {
    TYPE_CODES code = (TYPE_CODES) NEXT_CODE(vm);

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
    TYPE_CODES code = (TYPE_CODES) NEXT_CODE(vm);

    switch (code) {
        case TYPE_INT: {
            int rhs = pop_int(vm);
            int lhs = pop_int(vm);
            push_int(vm, lhs + rhs);
        }
            break;
        case TYPE_DOUBLE: {
            double rhs = pop_double(vm);
            double lhs = pop_double(vm);
            push_double(vm, lhs + rhs);
        }
            break;
        default:
            break;
    }
}

static void op_sub(vm_t *vm) {
    TYPE_CODES code = (TYPE_CODES) NEXT_CODE(vm);

    switch (code) {
        case TYPE_INT: {
            int rhs = pop_int(vm);
            int lhs = pop_int(vm);
            push_int(vm, lhs - rhs);
        }
            break;
        case TYPE_DOUBLE: {
            double rhs = pop_double(vm);
            double lhs = pop_double(vm);
            push_double(vm, lhs - rhs);
        }
            break;
        default:
            break;
    }
}

static void op_mul(vm_t *vm) {
    TYPE_CODES code = (TYPE_CODES) NEXT_CODE(vm);

    switch (code) {
        case TYPE_INT: {
            int rhs = pop_int(vm);
            int lhs = pop_int(vm);
            push_int(vm, lhs * rhs);
        }
            break;
        case TYPE_DOUBLE: {
            double rhs = pop_double(vm);
            double lhs = pop_double(vm);
            push_double(vm, lhs * rhs);
        }
            break;
        default:
            break;
    }
}

static void op_div(vm_t *vm) {
    TYPE_CODES code = (TYPE_CODES) NEXT_CODE(vm);

    switch (code) {
        case TYPE_INT: {
            int rhs = pop_int(vm);
            int lhs = pop_int(vm);
            if (rhs == 0 || lhs == 0) {
                THROW_ERROR(vm, ERR_DIVIDE_BY_ZERO);
            }

            push_int(vm, lhs / rhs);
        }
            break;
        case TYPE_DOUBLE: {
            double rhs = pop_double(vm);
            double lhs = pop_double(vm);

            if (rhs == 0 || lhs == 0) {
                THROW_ERROR(vm, ERR_DIVIDE_BY_ZERO);
            }

            push_double(vm, lhs / rhs);
        }
            break;
        default:
            break;
    }
}

static void op_cast(vm_t *vm) {
    TYPE_CODES from_type = (TYPE_CODES) NEXT_CODE(vm);
    TYPE_CODES to_type = (TYPE_CODES) NEXT_CODE(vm);

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
    TYPE_CODES from_type = (TYPE_CODES) NEXT_CODE(vm);

    if (from_type == TYPE_INT) {
        int data = pop_int(vm);
        push_int(vm, -data);
    } else if (from_type == TYPE_DOUBLE) {
        double data = pop_double(vm);
        push_int(vm, -data);
    }
}

static void op_dbg_print(vm_t *vm) {
    TYPE_CODES code = (TYPE_CODES) NEXT_CODE(vm);
    switch (code) {
        case TYPE_INT:
            wprintf(L"%d\n", pop_int(vm));
            break;
        case TYPE_DOUBLE:
            wprintf(L"%f\n", pop_double(vm));
            break;
    }
}

void run_vm(vm_t *vm) {
    bool running = true;

    do {
        if (vm->reg[REG_MACHINE_STATUS] != STATUS_GOOD) {
            running = false;
            break;
        }

        char opcode = NEXT_CODE(vm);
        switch (opcode) {
            case OP_HALT:
                wprintf(L"프로그램 종료.\n");
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
    } while (running);
}