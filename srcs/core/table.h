//
// Created by jiney on 2019-03-19.
//

#ifndef BOM_TABLE_H
#define BOM_TABLE_H

#include "common.h"

/**
 * @brief   테이블의 내부 정보를 가지고있는 구조체
 */
typedef struct table_pair table_pair_t;

/**
 * @brief   테이블 해더
 */
typedef struct table table_t;

/**
 * @brief   테이블의 내부 정보를 가지고있는 구조체
 */
struct table_pair {
    u16char *key;       /* < 키 */
    void *data;         /* < 값 */

    table_pair_t *next; /* < 해시값에서 중복되는 다음 값 */
};

/**
 * @brief   테이블 해더
 */
struct table {
    size_t len;             /* < 테이블 정보 량 */
    size_t cap;             /* < 테이블 정보에 할당된 량 */
    table_pair_t **pairs;   /* < 테이블 정보 */
};

/**
 * @brief   주어진 테이블을 초기화한다
 * @param   table   초기화할 테이블
 */
DLL_EXPORT void init_table(table_t *table);

/**
 * @brief   주어진 테이블을 해제한다
 * @param   table   해제할 테이블
 */
DLL_EXPORT void final_table(table_t *table);

/**
 * @brief   주어진 테이블에서 키로 값을 찾는다
 * @param   table   찾을 테이블
 * @param   key     찾을 값
 */
DLL_EXPORT table_pair_t *table_find_by_key(table_t *table, const u16char *key);

/**
 * @brief   주어진 테이블에 키와 값을 집어넣는다
 * @param   table   집어 넣을 테이블
 * @param   key     넣을 키
 * @param   data    넣을 값
 */
DLL_EXPORT table_pair_t *table_insert_data(table_t *table, const u16char *key, void *data);

#endif //BOM_TABLE_H
