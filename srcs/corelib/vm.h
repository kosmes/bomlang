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

struct vm {
    union {
        struct {
            unsigned char *data;
            unsigned char *text;
        };
        script_t script;
    };

    union  {
        unsigned char full_mem[MEMORY_SIZE];

        struct {
            unsigned short reg[16];
            unsigned char stack[STACK_SIZE];
            unsigned short mem[0];
        };
    };
};

DLL_EXPORT void init_vm(vm_t *vm);

DLL_EXPORT void final_vm(vm_t *vm);

DLL_EXPORT void set_script(vm_t *vm, script_t *script);

DLL_EXPORT void run_vm(vm_t *vm);

#endif //BOM_VM_H
