//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_VM_H
#define BOM_VM_H

#include "common.h"
#include "runtime.h"

#include "compiler/script.h"
#include "compiler/compiler.h"

#define MEMORY_SIZE 0x4096
#define STACK_SIZE 0x24

/**
 * @brief   가상머신에 관련한 정보를 가지고있는 구조체
 */
typedef struct vm Machine;

/**
 * @brief   사용하는 레지스터를 모아놓은 열거형
 */
enum REGISTERS {
    REG_PROGRAM_CODE,
    REG_STACK_POINTER,
    REG_FRAME_POINTER,
    REG_MACHINE_STATUS,

    UNUSED_REG,
};

/**
 * @brief   머신의 상태에 관한 것들을 모아놓은 열거형
 */
enum MACHINE_STATUS {
    STATUS_GOOD,
    STATUS_ERROR_THROWN,
};

/**
 * @brief   가상머신에 관련한 정보를 가지고있는 구조체
 */
struct vm {
    union {
        struct {
            unsigned char *data;
            unsigned char *text;
        };
        Script script;
    };

    unsigned short reg[16];     /* < 레지스터 */
    Var stack[STACK_SIZE];    /* < 스택 */
    Var local[STACK_SIZE];    /* < 로컬 변수 리스트 */

    Compiler *compiler;
};

/**
 * @brief   가상 머신을 생성한다
 */
DLL_EXPORT Machine *MachineCreate();

DLL_EXPORT void MachineDestroy(Machine *machine);

/**
 * @brief   주어진 가상 머신에 스크립트를 연결시킨다
 * @detail 
 * 이전에 연결한 스크립트가 있을 경우 해제시키고 새로 연결한다.
 */
DLL_EXPORT void MachineSetScript(Machine *vm, Script *script);

/**
 * @brief   가상 머신에서 스크립트를 실행한다
 */
DLL_EXPORT void MachineRun(Machine *vm);

DLL_EXPORT void MachineRunCode(Machine *vm, const u16char *code);

#endif //BOM_VM_H
