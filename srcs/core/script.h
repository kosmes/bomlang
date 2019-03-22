//
// Created by jiney on 2019-03-17.
//

#ifndef BOM_SCRIPT_H
#define BOM_SCRIPT_H

#include "common.h"

typedef struct script Script;

struct script {
    unsigned char *data;
    unsigned char *text;
};

/**
 * @brief   주어진 스크립트를 초기화한다
 * @param   script  초기화 시킬 스크립트
 */
DLL_EXPORT void ScriptInit(Script *script);

/**
 * @brief   주어진 스크립트를 해제시킨다
 * @param   script  해제 시킬 스크립트
 */
DLL_EXPORT void ScriptFinal(Script *script);

/**
 * @brief   주어진 스크립트에 정수형 값을 집어넣고 집어넣은 위치를 반환한다
 * @param   script  집어 넣을 스크립트
 * @param   data    집어 넣을 값
 * @return  집어넣은 위치
 */
DLL_EXPORT size_t ScriptAddIntAndReturnAddr(Script *script, int data);

/**
 * @brief   주어진 스크립트에 실수형 값을 집어넣고 집어넣은 위치를 반환한다
 * @param   script  집어 넣을 스크립트
 * @param   data    집어 넣을 값
 * @return  집어넣은 위치
 */
DLL_EXPORT size_t ScriptAddDoubleAndReturnAddr(Script *script, double data);

/**
 * @brief   주어진 스크립트에 문자열 값을 집어넣고 집어넣은 위치를 반환한다
 * @param   script  집어 넣을 스크립트
 * @param   data    집어 넣을 문자열
 * @return  집어넣은 위치
 */
DLL_EXPORT size_t ScriptAddStringAndReturnAddr(Script *script, const wchar_t *data);

/**
 * @brief   주어진 스크립트에서 해당 위치에 정수형 값을 가져온다
 * @param   script  가져올 스크립트
 * @param   addr    가져올 위치
 * @return  정수형 값
 */
DLL_EXPORT int ScriptGetIntFromAddr(Script *script, size_t addr);

/**
 * @brief   주어진 스크립트에서 해당 위치에 실수형 값을 가져온다
 * @param   script  가져올 스크립트
 * @param   addr    가져올 위치
 * @return  실수형 값
 */
DLL_EXPORT double ScriptGetDoubleFromAddr(Script *script, size_t addr);

/**
 * @brief   주어진 스크립트에서 해당 위치에 문자열 값을 가져온다
 * @param   script  가져올 스크립트
 * @param   addr    가져올 위치
 * @return  문자열 값
 */
DLL_EXPORT const wchar_t *ScriptGetStringFromAddr(Script *script, size_t addr);

DLL_EXPORT void ScriptPrint(Script *script);

#endif //BOM_SCRIPT_H
