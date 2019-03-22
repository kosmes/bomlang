//
// Created by jiney on 2019-03-17.
//

#include "vm.h"
#include "buf.h"
#include "error.h"

#define NEXT_CODE(vm) ((vm)->text[(vm)->reg[REG_PROGRAM_CODE]++])
#define THROW_ERROR(vm, errcode) (ErrorLine(errcode, 0), \
        (vm)->reg[REG_MACHINE_STATUS] = STATUS_ERROR_THROWN); return;

#define CHECK_STACK_OVERFLOW(vm, offset) if ((vm)->reg[REG_STACK_POINTER] + offset >= STACK_SIZE) \
    { ErrorLine(ERR_STACK_OVERFLOW, 0); return; }

#define CHECK_STACK_UNDERFLOW(vm, offset) if ((vm)->reg[REG_STACK_POINTER] - offset < 0) \
    { ErrorLine(ERR_STACK_UNDERFLOW, 0); return 0; }

#define GET_STACK(vm, offset) ((vm)->stack[(vm)->reg[REG_STACK_POINTER] - offset - 1])

void init_vm(vm_t *vm) {
    vm->reg[REG_PROGRAM_CODE] = 0;
    vm->reg[REG_FRAME_POINTER] = 0;
    vm->reg[REG_STACK_POINTER] = 0;
    vm->reg[REG_MACHINE_STATUS] = 0;

    vm->data = NULL;
    vm->text = NULL;

    for (int i = 0; i < STACK_SIZE; i++) {
        vm->stack[i].type_id = TYPE_NONE;
        vm->stack[i].data = NULL;

        vm->local[i].type_id = TYPE_NONE;
        vm->local[i].data = NULL;
    }
}

void final_vm(vm_t *vm) {
    buf_free(vm->data);
    buf_free(vm->text);

    for (int i = 0; i < STACK_SIZE; i++) {
        if (vm->stack[i].data != NULL) {
            free(vm->stack[i].data);
        }
    }
}

void set_script(vm_t *vm, Script *script) {
    ScriptFinal(&vm->script);
    vm->reg[REG_PROGRAM_CODE] = 0;

    for (int i = 0; i < buf_len(script->data); i++) {
        buf_push(vm->data, script->data[i]);
    }

    for (int i = 0; i < buf_len(script->text); i++) {
        buf_push(vm->text, script->text[i]);
    }
}

static void push_int(vm_t *vm, int data) {
    CHECK_STACK_OVERFLOW(vm, 1);

    Var var = vm->stack[vm->reg[REG_STACK_POINTER]];
    if (var.data != NULL) {
        free(var.data);
    }
    var.data = malloc(sizeof(int));
    memcpy(var.data, &data, sizeof(int));
    var.type_id = TYPE_INT;
    vm->stack[vm->reg[REG_STACK_POINTER]] = var;

    vm->reg[REG_STACK_POINTER]++;
}

static void push_double(vm_t *vm, double data) {
    CHECK_STACK_OVERFLOW(vm, 1);

    Var var = vm->stack[vm->reg[REG_STACK_POINTER]];
    if (var.data != NULL) {
        free(var.data);
    }
    var.data = malloc(sizeof(double));
    memcpy(var.data, &data, sizeof(double));
    var.type_id = TYPE_DOUBLE;
    vm->stack[vm->reg[REG_STACK_POINTER]] = var;

    vm->reg[REG_STACK_POINTER]++;
}

static int pop_int(vm_t *vm) {
    CHECK_STACK_UNDERFLOW(vm, 1);

    vm->reg[REG_STACK_POINTER]--;
    Var var = vm->stack[vm->reg[REG_STACK_POINTER]];
    if (var.data == NULL) {
        Error(ERR_INVALID_ACCESS);
        return 0;
    }
    int data = *((int *) var.data);
    return data;
}

static double pop_double(vm_t *vm) {
    CHECK_STACK_UNDERFLOW(vm, 1);

    vm->reg[REG_STACK_POINTER]--;
    Var var = vm->stack[vm->reg[REG_STACK_POINTER]];
    if (var.data == NULL) {
        Error(ERR_INVALID_ACCESS);
        return 0;
    }
    double data = *((double *) var.data);
    return data;
}

static TYPE_ID cast_stack_2value_equal_from_top(vm_t *vm) {
    TYPE_ID first = GET_STACK(vm, 0).type_id;
    TYPE_ID second = GET_STACK(vm, 1).type_id;

    if (first == second) {
        return first;
    }

    TYPE_ID result_type = TYPE_NONE;

    switch (first) {
        case TYPE_INT: {
            int first_data = pop_int(vm);
            if (second == TYPE_DOUBLE) {
                result_type = TYPE_INT;
                push_int(vm, (int) pop_double(vm));
            }
            push_int(vm, first_data);
            break;
        }
        case TYPE_DOUBLE: {
            double first_data = pop_double(vm);
            if (second == TYPE_INT) {
                result_type = TYPE_DOUBLE;
                push_double(vm, (double) pop_int(vm));
            }
            push_double(vm, first_data);
            break;
        }
        default:
            break;
    }

    return result_type;
}

static void op_const(vm_t *vm) {
    Converter cvt;
    cvt.asDouble = 0;
    for (int i = 0; i < 8; i++) {
        cvt.as_bytes[i] = NEXT_CODE(vm);
    }

    TYPE_ID type_id = (TYPE_ID) vm->data[cvt.as_size++];

    switch (type_id) {
        case TYPE_INT:
            push_int(vm, ScriptGetIntFromAddr(&vm->script, cvt.as_size));
            break;
        case TYPE_DOUBLE:
            push_double(vm, ScriptGetDoubleFromAddr(&vm->script, cvt.as_size));
            break;
        default:
            break;
    }
}

static void op_store(vm_t *vm) {
    TYPE_ID type = GET_STACK(vm, 0).type_id;
    Converter cvt;
    for (int i = 0; i < 8; i++) {
        cvt.as_bytes[i] = NEXT_CODE(vm);
    }
    size_t addr = cvt.as_size;
    Var var = vm->local[addr];
    if (var.data != NULL) {
        free(var.data);
    }

    switch (type) {
        case TYPE_INT: {
            int data = pop_int(vm);
            var.data = malloc(sizeof(int));
            memcpy(var.data, &data, sizeof(int));
            var.type_id = TYPE_INT;
            break;
        }
        case TYPE_DOUBLE: {
            double data = pop_double(vm);
            var.data = malloc(sizeof(double));
            memcpy(var.data, &data, sizeof(double));
            var.type_id = TYPE_DOUBLE;
            break;
        }
        default:
            break;
    }

    vm->local[addr] = var;
}

static void op_load(vm_t *vm) {
    Converter cvt;
    for (int i = 0; i < 8; i++) {
        cvt.as_bytes[i] = NEXT_CODE(vm);
    }
    size_t addr = cvt.as_size;
    Var var = vm->local[addr];
    switch (var.type_id) {
        case TYPE_INT:
            push_int(vm, *((int *) var.data));
            break;
        case TYPE_DOUBLE:
            push_double(vm, *((double *) var.data));
            break;
        default:
            break;
    }
}

static void op_add(vm_t *vm) {
    TYPE_ID type = cast_stack_2value_equal_from_top(vm);

    switch (type) {
        case TYPE_INT: {
            int rhs = pop_int(vm);
            int lhs = pop_int(vm);
            push_int(vm, lhs + rhs);
            break;
        }
        case TYPE_DOUBLE: {
            double rhs = pop_double(vm);
            double lhs = pop_double(vm);
            push_double(vm, lhs + rhs);
            break;
        }
        default:
            break;
    }
}

static void op_sub(vm_t *vm) {
    TYPE_ID type = cast_stack_2value_equal_from_top(vm);

    switch (type) {
        case TYPE_INT: {
            int rhs = pop_int(vm);
            int lhs = pop_int(vm);
            push_int(vm, lhs - rhs);
            break;
        }
        case TYPE_DOUBLE: {
            double rhs = pop_double(vm);
            double lhs = pop_double(vm);
            push_double(vm, lhs - rhs);
            break;
        }
        default:
            break;
    }
}

static void op_mul(vm_t *vm) {
    TYPE_ID type = cast_stack_2value_equal_from_top(vm);

    switch (type) {
        case TYPE_INT: {
            int rhs = pop_int(vm);
            int lhs = pop_int(vm);
            push_int(vm, lhs * rhs);
            break;
        }
        case TYPE_DOUBLE: {
            double rhs = pop_double(vm);
            double lhs = pop_double(vm);
            push_double(vm, lhs * rhs);
            break;
        }
        default:
            break;
    }
}

static void op_div(vm_t *vm) {
    TYPE_ID type = cast_stack_2value_equal_from_top(vm);

    switch (type) {
        case TYPE_INT: {
            int rhs = pop_int(vm);
            int lhs = pop_int(vm);
            if (lhs == 0) {
                THROW_ERROR(vm, ERR_DIVIDE_BY_ZERO);
                return;
            }
            push_int(vm, lhs - rhs);
            break;
        }
        case TYPE_DOUBLE: {
            double rhs = pop_double(vm);
            double lhs = pop_double(vm);
            if (lhs == 0) {
                THROW_ERROR(vm, ERR_DIVIDE_BY_ZERO);
                return;
            }
            push_double(vm, lhs - rhs);
            break;
        }
        default:
            break;
    }
}

static void op_invert(vm_t *vm) {
    TYPE_ID type = GET_STACK(vm, 0).type_id;
    switch (type) {
        case TYPE_INT:
            push_int(vm, -pop_int(vm));
            break;
        case TYPE_DOUBLE:
            push_double(vm, -pop_double(vm));
            break;

        default:
            break;
    }
}

static void op_return(vm_t *vm) {

}

static void op_dbg_print(vm_t *vm) {
    TYPE_ID code = GET_STACK(vm, 0).type_id;
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

        OP_CODE opcode = NEXT_CODE(vm);
        switch (opcode) {
            case OP_HALT:
                wprintf(L"프로그램 종료.\n");
                running = false;
                break;

            case OP_CONST:
                op_const(vm);
                break;

            case OP_STORE:
                op_store(vm);
                break;

            case OP_LOAD:
                op_load(vm);
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
                // op_cast(vm);
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