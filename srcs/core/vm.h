//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_VM_H
#define BOM_VM_H

#include "common.h"
#include "runtime.h"
#include "script.h"

#define MEMORY_SIZE 0x4096
#define STACK_SIZE 0x24

typedef struct var var_t;
typedef struct vm vm_t;

enum REGISTERS {
    REG_PROGRAM_CODE,
    REG_STACK_POINTER,
    REG_FRAME_POINTER,
    REG_MACHINE_STATUS,

    UNUSED_REG,
};

enum MACHINE_STATUS {
    STATUS_GOOD,
    STATUS_ERROR_THROWN,
};

struct var {
    TYPE_IDS type_id;
    void *data;
};

struct vm {
    union {
        struct {
            unsigned char *data;
            unsigned char *text;
        };
        script_t script;
    };

    unsigned short reg[16];
    var_t stack[STACK_SIZE];
    var_t local[STACK_SIZE];
};

DLL_EXPORT void init_vm(vm_t *vm);

DLL_EXPORT void final_vm(vm_t *vm);

DLL_EXPORT void set_script(vm_t *vm, script_t *script);

DLL_EXPORT void run_vm(vm_t *vm);

#endif //BOM_VM_H
