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

/**
 * @brief   아무 정보 형이든 다 가질 수 있는 구조체
 */
typedef struct var var_t;

/**
 * @brief   가상머신에 관련한 정보를 가지고있는 구조체
 */
typedef struct vm vm_t;

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
 * @brief   아무 정보 형이든 다 가질 수 있는 구조체
 */
struct var {
    TYPE_IDS type_id;   /* < 변수 종류 */
    void *data;         /* < 변수 값 */
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
        script_t script;
    };

    unsigned short reg[16];     /* < 레지스터 */
    var_t stack[STACK_SIZE];    /* < 스택 */
    var_t local[STACK_SIZE];    /* < 로컬 변수 리스트 */
};

/**
 * @brief   주어진 가상 머신을 초기화한다
 */
DLL_EXPORT void init_vm(vm_t *vm);

/**
 * @brief   주어진 가상 머신을 해제시킨다
 */
DLL_EXPORT void final_vm(vm_t *vm);

/**
 * @brief   주어진 가상 머신에 스크립트를 연결시킨다
 * @detail 
 * 이전에 연결한 스크립트가 있을 경우 해제시키고 새로 연결한다.
 */
DLL_EXPORT void set_script(vm_t *vm, script_t *script);

/**
 * @brief   가상 머신에서 스크립트를 실행한다
 */
DLL_EXPORT void run_vm(vm_t *vm);

#endif //BOM_VM_H
